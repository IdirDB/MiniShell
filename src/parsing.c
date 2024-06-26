#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parsing.h"

extern char **environ; 

char* interpretQuotes(const char *input) {
    int inSingleQuotes = 0;
    int inDoubleQuotes = 0;
    int outputIndex = 0; // Index to keep track of position in output buffer
    int outputCapacity = 256; // Initial capacity for the output buffer

    // Buffer to store the interpreted output
    char *output = calloc(outputCapacity, sizeof(char));
    if (output == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    int len = strlen(input);
    printf("%s\n", input);
    
    for (int i = 0; i < len; i++) {
        char currentChar = input[i];

        // Check if we need to expand the output buffer
        if (outputIndex >= outputCapacity - 1) {
            outputCapacity *= 2;
            output = realloc(output, outputCapacity * sizeof(char));
            if (output == NULL) {
                fprintf(stderr, "Memory allocation failed.\n");
                exit(1);
            }
        }

        if (currentChar == '\'' && !inDoubleQuotes) {
            inSingleQuotes = !inSingleQuotes;
            continue;
        } else if (currentChar == '"' && !inSingleQuotes) {
            inDoubleQuotes = !inDoubleQuotes;
            continue;
        } else {
            if (inSingleQuotes) {
                output[outputIndex++] = currentChar; // Inside single quotes: everything is literal
            } else if (inDoubleQuotes) {
                if (currentChar == '\\') { // Inside double quotes: handle escape sequences and variables
                    if (i + 1 < len) {
                        i++;
                        switch (input[i]) {
                            case '$':
                                output[outputIndex++] = '$';
                                break;
                            case 'n':
                                output[outputIndex++] = '\n';
                                break;
                            case 't':
                                output[outputIndex++] = '\t';
                                break;
                            case '\"':
                                output[outputIndex++] = '"';
                                break;
                            case '\\':
                                output[outputIndex++] = '\\';
                                break;
                            default:
                                output[outputIndex++] = '\\';
                                printf("%d", outputIndex);
                                break;
                        }
                    }
                } else if (currentChar == '$') {
                    // Handle environment variables
                    int envVariableBegin = i + 1;
                    int envVariableEnd = envVariableBegin;

                    // Find the end of the variable name
                    while (envVariableEnd < len && (isalnum(input[envVariableEnd]) || input[envVariableEnd] == '_')) {
                        envVariableEnd++;
                    }

                    // Extract the variable name
                    int varNameLength = envVariableEnd - envVariableBegin;
                    char *envVariable = malloc((varNameLength + 1) * sizeof(char));
                    if (envVariable == NULL) {
                        fprintf(stderr, "Memory allocation failed.\n");
                        exit(1);
                    }
                    strncpy(envVariable, input + envVariableBegin, varNameLength);
                    envVariable[varNameLength] = '\0';

                    // Lookup the variable value
                    char *result = getenv(envVariable);
                    printf("%s\n", result);
                    free(envVariable);

                    // Append the result to the output if found
                    if (result != NULL) {
                        int resultLen = strlen(result);
                        for (int j = 0; j < resultLen; j++) {
                            // Check if we need to expand the output buffer
                            if (outputIndex >= outputCapacity - 1) {
                                outputCapacity *= 2;
                                output = realloc(output, outputCapacity * sizeof(char));
                                if (output == NULL) {
                                    fprintf(stderr, "Memory allocation failed.\n");
                                    exit(1);
                                }
                            }
                            printf("%d : %s\n", outputIndex, output);
                            output[outputIndex++] = result[j];
                        }
                    } else {
                        // If the variable is not found, we should add the original variable string
                        output[outputIndex++] = '$';
                        for (int k = envVariableBegin; k < envVariableEnd; k++) {
                            output[outputIndex++] = input[k];
                        }
                    }

                    // Move the index to the end of the variable name
                    i = envVariableEnd - 1;
                } else {
                    // Normal character inside double quotes
                    printf("%d** : %c\n", outputIndex, currentChar);
                    output[outputIndex++] = currentChar;
                }
            } else {
                // Outside of any quotes: treat characters normally
                output[outputIndex++] = currentChar;
            }
        }
    }

    // Null-terminate the output string
    output[outputIndex] = '\0';

    // Resize output to fit the actual size
    output = realloc(output, (outputIndex + 1) * sizeof(char));
    if (output == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    return output;
}

/* Parsing : cmd1; cmd2; cmd3;
Example : >ls | cat; cd idir; -> [["ls | cat"], ["cd idir"]]
*/
char** parseLigne(char* ligne) {
    char* start = ligne;
    char* end = ligne;
    char** listCmd = NULL;
    size_t index = 0;
    char delim = ';';

    while (*end != '\0'){
        if (*end == '\\' && *(end + 1) == delim) {
            // Skip the escaped delimiter
            memmove(end, end + 1, strlen(end));
        } else if (*end == delim) {
            // Found a delimiter
            listCmd = (char**) realloc(listCmd, (index + 1) * sizeof(char*));
            if (listCmd == NULL) {
                fprintf(stderr, "Memory allocation failed");
                exit(EXIT_FAILURE);
            }
            listCmd[index] = strndup(start, end - start);
            if (listCmd[index] == NULL) {
                fprintf(stderr, "Memory allocation failed");
                exit(EXIT_FAILURE);
            }
            start = end + 1; // Move to the next character after the delimiter
            index++;
        }
        end++;
    }

    // Add the last token
    listCmd = (char**) realloc(listCmd, (index + 1) * sizeof(char*));
    if (listCmd == NULL) {
        fprintf(stderr, "Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    listCmd[index] = strdup(start);
    if (listCmd[index] == NULL) {
        fprintf(stderr, "Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    index++;

    // Add NULL at the end 
    listCmd = (char**) realloc(listCmd, (index + 1) * sizeof(char*));
    if (listCmd == NULL) {
        fprintf(stderr, "Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    listCmd[index] = NULL;

    return listCmd;
}  

/* Parsing : cmd1; cmd2; cmd3;
Example : >ls | cat; cd idir; -> [["ls | cat"], ["cd idir"]] -> [[[ls] -> [cat] -> [NULL]], [[cd idir] -> [NULL]]]
*/
Node* parseCmdPipe(char* ligne) {
    char* start = ligne;
    char* end = ligne;
    Node* headNode = NULL;
    Node* currentNode = NULL;
    char delim = '|';

    while (*end != '\0') {
        if (*end == '\\' && *(end + 1) == delim) {
            // Skip the escaped delimiter
            memmove(end, end + 1, strlen(end));
        } else if (*end == delim) {
            // Found a delimiter
            char* cmd = strndup(start, end - start);
            if (cmd == NULL) {
                fprintf(stderr, "Memory allocation failed");
                exit(EXIT_FAILURE);
            }
            if (headNode == NULL) {
                headNode = (Node*) malloc(sizeof(Node));
                if (headNode == NULL) {
                    fprintf(stderr, "Memory allocation failed");
                    exit(EXIT_FAILURE);
                }
                headNode->cmd = cmd;
                headNode->next = NULL;
                currentNode = headNode;
            } else {
                Node* newNode = (Node*) malloc(sizeof(Node));
                if (newNode == NULL) {
                    fprintf(stderr, "Memory allocation failed");
                    exit(EXIT_FAILURE);
                }
                newNode->cmd = cmd;
                newNode->next = NULL;
                currentNode->next = newNode;
                currentNode = newNode;
            }
            start = end + 1; 
        }
        end++;
    }

    // Add the last command
    if (*start != '\0') {
        char* cmd = strdup(start);
        if (cmd == NULL) {
            fprintf(stderr, "Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        if (headNode == NULL) {
            headNode = (Node*) malloc(sizeof(Node));
            if (headNode == NULL) {
                fprintf(stderr, "Memory allocation failed");
                exit(EXIT_FAILURE);
            }
            headNode->cmd = cmd;
            headNode->next = NULL;
        } else {
            Node* newNode = (Node*) malloc(sizeof(Node));
            if (newNode == NULL) {
                fprintf(stderr, "Memory allocation failed");
                exit(EXIT_FAILURE);
            }
            newNode->cmd = cmd;
            newNode->next = NULL;
            currentNode->next = newNode;
        }
    }

    return headNode;
}
  
/* Parsing : cmd1 arg1 arg2 
Example : [echo "idir"] -> [["echo"], ["idir"]]
*/
char** parseCmd(char* cmd) {
    char* token = NULL;
    char** tokenList = NULL;
    size_t index = 0;
    int simpleQuoteMode = 0;
    int doubleQuoteMode = 0;
    size_t length = strlen(cmd);
    char* redirectionWrite = ">";
    char* redirectionRead = "<";
    char* redirectionDoubleWrite = ">>"; 
    size_t strBegin = 0;
    size_t strEnd = 0;

    for (size_t i = 0; i < length; i++) {
    	//The begening of a new token
        if (cmd[i] != ' ') { 
            strBegin = i;
            strEnd = strBegin;
            
            if (cmd[i] == '\'') {
                simpleQuoteMode = !simpleQuoteMode;
                strEnd++;
            } else if (cmd[i] == '\"') {
                doubleQuoteMode = !doubleQuoteMode;
                strEnd++;
            } 
            
            if (simpleQuoteMode) {
                while (strEnd < length && cmd[strEnd] != '\'') {
                    strEnd++;
                }
                strEnd++;
                simpleQuoteMode = !simpleQuoteMode;
            } else if (doubleQuoteMode) {
                while (strEnd < length && cmd[strEnd] != '\"') {
                    strEnd++;
                }
                strEnd++;
                doubleQuoteMode = !doubleQuoteMode;
            } else {
                while (strEnd < length && !isspace(cmd[strEnd])) {
                    strEnd++;
                }
           	}	

            // Allocate memory for the token
            token = (char*) malloc((strEnd - strBegin) * sizeof(char));
            if (token == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }

            // Copy the token
            strncpy(token, cmd + strBegin, strEnd - strBegin);
            token[strEnd - strBegin] = '\0';

            // Add the token to tokenList
            tokenList = (char**) realloc(tokenList, (index + 1) * sizeof(char*));
            if (tokenList == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }

            // Check for redirections and handle them
			if (token[0] == '>' && token[1] == '>') {
				tokenList[index] = strdup(redirectionDoubleWrite);
				if (token[2] != '\0') {
					char* restOfToken = strdup(token + 2); // Copy the rest of the token
					tokenList = (char**) realloc(tokenList, (index + 2) * sizeof(char*));
					if (tokenList == NULL) {
						fprintf(stderr, "Memory allocation failed\n");
						exit(EXIT_FAILURE);
					}
					tokenList[index++] = restOfToken; // Add the rest of the token to tokenList
				}
			} else if (token[0] == '>' && token[1] != '\0') {
				tokenList[index] = strdup(redirectionWrite);
				if (token[1] != '\0') {
					char* restOfToken = strdup(token + 1); // Copy the rest of the token
					tokenList = (char**) realloc(tokenList, (index + 2) * sizeof(char*));
					if (tokenList == NULL) {
						fprintf(stderr, "Memory allocation failed\n");
						exit(EXIT_FAILURE);
					}
					tokenList[++index] = restOfToken; // Add the rest of the token to tokenList
				}
			} else if (token[0] == '<' && token[1] != '\0') {
				tokenList[index] = strdup(redirectionRead);
				if (token[1] != '\0') {
					char* restOfToken = strdup(token + 1); // Copy the rest of the token
					tokenList = (char**) realloc(tokenList, (index + 2) * sizeof(char*));
					if (tokenList == NULL) {
						fprintf(stderr, "Memory allocation failed\n");
						exit(EXIT_FAILURE);
					}
					tokenList[index++] = restOfToken; // Add the rest of the token to tokenList
				}
			} else {
				tokenList[index++] = strdup(interpretQuotes(strdup(token)));
			}

            i = strEnd - 1; // Move to the end of the current token
        }
    }

    // Add NULL to the end
    tokenList = (char**) realloc(tokenList, (index + 1) * sizeof(char*));
    if (tokenList == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    tokenList[index] = NULL;

    return tokenList;
}

Env_variable* split(const char* str) {
    int len_first_part = 0;

    while (str[len_first_part] != '=' && str[len_first_part] != '\0') {
        len_first_part++;
    }

    if (str[len_first_part] != '=') {
        fprintf(stderr, "Error: The '=' character was not found in the string.\n");
        exit(EXIT_FAILURE);
    }

    Env_variable* env_variable = (Env_variable*) malloc(sizeof(Env_variable));
    if (env_variable == NULL) {
        fprintf(stderr, "Memory allocation failed for env_variable");
        exit(EXIT_FAILURE);
    }

    env_variable->first_part = (char*) malloc((len_first_part + 1) * sizeof(char));
    if (env_variable->first_part == NULL) {
        fprintf(stderr, "Memory allocation failed for first_part");
        free(env_variable);
        exit(EXIT_FAILURE);
    }

    int len_second_part = strlen(str) - len_first_part - 1;
    env_variable->second_part = (char*) malloc((len_second_part + 1) * sizeof(char));
    if (env_variable->second_part == NULL) {
        fprintf(stderr, "Memory allocation failed for second_part");
        free(env_variable->first_part);
        free(env_variable);
        exit(EXIT_FAILURE);
    }

    strncpy(env_variable->first_part, str, len_first_part);
    env_variable->first_part[len_first_part] = '\0';
    
    strcpy(env_variable->second_part, str + len_first_part + 1);
    env_variable->second_part[len_second_part] = '\0'; 

    return env_variable;
}

char* strndup(const char* s, size_t n) {
    char* result;
    size_t len = strlen(s);
    
    if (n < len) 
    	len = n;
    result = (char*) malloc(len + 1);
    if (result == NULL) 
    	return NULL;
    result[len] = '\0';
    return (char*) memcpy(result, s, len);
}	   

