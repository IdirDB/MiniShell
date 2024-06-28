#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parsing.h"
#include "environ.h"

/**
 * Interprète les tokens, traite les séquences d'échappement et gère les variables d'environnement.
 * 
 * @param input Le token d'entré à interpréter.
 * @return Le token interprèté.
 */
char* interpretQuotes(const char *input) {
    int inSingleQuotes = 0;
    int inDoubleQuotes = 0;
    int outputIndex = 0; 
    int outputCapacity = 256; 

    // buffer pour stocker la sortie interprétée
    char *output = calloc(outputCapacity, sizeof(char));
    if (output == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    int len = strlen(input);
    
    for (int i = 0; i < len; i++) {
        char currentChar = input[i];

        // Check si on doit étendre output buffer
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
        	// Tout est littéral.
            if (inSingleQuotes) {
                output[outputIndex++] = currentChar;
            // Interprétation des caractères spéciaux     
            } else {
                if (currentChar == '\\') { 
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
                                break;
                        }
                    }
                    // Prendre en compte les variables d'environnement
                } else if (currentChar == '$') {
                    int envVariableBegin = i + 1;
                    int envVariableEnd = envVariableBegin;

                    while (envVariableEnd < len && (isalnum(input[envVariableEnd]) || input[envVariableEnd] == '_')) {
                        envVariableEnd++;
                    }
				
                    int varNameLength = envVariableEnd - envVariableBegin;
                    char *envVariable = malloc((varNameLength + 1) * sizeof(char));
                    if (envVariable == NULL) {
                        fprintf(stderr, "Memory allocation failed.\n");
                        exit(1);
                    }
                    strncpy(envVariable, input + envVariableBegin, varNameLength);
                    envVariable[varNameLength] = '\0';

                    char *result = getenv(envVariable);
                    free(envVariable);

                    if (result != NULL) {
                        int resultLen = strlen(result);
                        for (int j = 0; j < resultLen; j++) {
							// Check si on doit étendre output buffer
                            if (outputIndex >= outputCapacity - 1) {
                                outputCapacity *= 2;
                                output = realloc(output, outputCapacity * sizeof(char));
                                if (output == NULL) {
                                    fprintf(stderr, "Memory allocation failed.\n");
                                    exit(1);
                                }
                            }
                            output[outputIndex++] = result[j];
                        }
                    } else {
                        // Si ce n'est pas une variable globale
                        output[outputIndex++] = '$';
                        for (int k = envVariableBegin; k < envVariableEnd; k++) {
                            output[outputIndex++] = input[k];
                        }
                    }
                    i = envVariableEnd - 1;
                } else {
                    output[outputIndex++] = currentChar;
                }
        	}
    	}
	}
	
	//Pour marquer la fin de output
	output[outputIndex] = '\0';

	return output;
}

/**
 * @brief Parse une ligne de commande en une liste de commandes séparées par le délimiteur ';'.
 *
 * Cette fonction prend une chaîne de caractères "ligne" représentant une série de commandes
 * séparées par le délimiteur ';'. Elle découpe la chaîne en tokens individuels représentant
 * chaque commande et les stocke dans un tableau de chaînes de caractères "listCmd", le tableau 
 * est terminé par NULL.
 *
 * @param ligne La chaîne de caractères représentant la ligne de commande à analyser.
 * @return Un tableau dynamique de chaînes de caractères contenant les commandes séparées, NULL en cas d'erreur.
 *
 * @note ';' peut être échappé avec '\\' pour être traités littéralement.
 *
 * Exemple : 
 * buffer = "cmd1 | cmd2; cmd3" -> bufferParsed = ["cmd1 | cmd2", "cmd3"].
 */
char** parseLigne(char* ligne) {
    char* start = ligne;
    char* end = ligne;
    char** listCmd = NULL;
    size_t index = 0;
    char delim = ';';

    while (*end != '\0'){
    	// Si ';' est échappé 
        if (*end == '\\' && *(end + 1) == delim) {
            memmove(end, end + 1, strlen(end));
        // Si ';' est pas échappé 
        } else if (*end == delim) {
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
            start = end + 1; 
            index++;
        }
        end++;
    }

    // Ajoute le dernier token à listCmd
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

    // Ajoute NULL pour terminer listCmd 
    listCmd = (char**) realloc(listCmd, (index + 1) * sizeof(char*));
    if (listCmd == NULL) {
        fprintf(stderr, "Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    listCmd[index] = NULL;

    return listCmd;
}  

/**
 * @brief Parse une ligne de commande pour la transformer en pipeline.
 *
 * Cette fonction prend une chaîne de caractères "ligne" représentant une série de commandes séparées par '|' ou pas.
 * Elle découpe la chaîne en commandes individuelles et les organise dans une liste chaînée de structures "Node".
 *
 * @param ligne La chaîne de caractères représentant la ligne de commande à analyser.
 * @return Un pointeur vers le premier nœud de la liste chaînée "Node" contenant les commandes séparées, NULL en cas d'erreur.
 *
 * @note Les délimiteurs peuvent être échappés avec '\\' pour être traités littéralement.
 *
 * Example :
 * bufferParsed = ["cmd1 | cmd2", "cmd3"] -> commandParsed = [linkedList(["cmd1"] -> ["cmd2"] -> [NULL]), linkedList(["cmd3"] -> [NULL])]
 */

Node* parseCmdPipe(char* ligne) {
    char* start = ligne;
    char* end = ligne;
    Node* headNode = NULL;
    Node* currentNode = NULL;
    char delim = '|';

    while (*end != '\0') {
        if (*end == '\\' && *(end + 1) == delim) {
            // Si '|' est échappé 
            memmove(end, end + 1, strlen(end));
        } else if (*end == delim) {
            // Si '|' est pas échappé 
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

    // Ajoute le dernier token à listCmd
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
  
/**
 * @brief Parse une ligne de commande en tokens en tenant compte des guillemets simples et doubles.
 *
 * Cette fonction prend une chaîne de caractères "cmd" représentant une ligne de commande et la découpe en tokens.
 * Les tokens sont séparés par des espaces mais les guillemets simples et doubles peuvent être utilisés pour encapsuler
 * des tokens contenant des espaces sans qu'ils ne soient divisés. Les redirections de fichiers vers et depuis des commandes sont
 * également détectées et gérées.
 *
 * @param cmd La chaîne de caractères représentant la ligne de commande à analyser.
 * @return Un tableau de chaînes de caractères "tokenList" contenant les tokens extraits, NULL en cas d'erreur.
 *
 * @note Les redirections de fichiers détectées incluent '>', '>>' pour l'écriture et '<' pour la lecture.
 *
 * Exemple :
 * Si cmd1 = echo "Hello" "World" -> cmdParsed = ["echo", "Hello", " ", "World", NULL]
 * Si echo "Hello" >a.txt -> cmdParsed = ["echo", "Hello", " ", ">", "a.txt"]
 */

char** parseCmd(char* cmd) {
    char* token = NULL;
    char** tokenList = NULL;
    size_t index = 0;
    size_t simpleQuoteMode = 0;
    size_t doubleQuoteMode = 0;
    size_t length = strlen(cmd);
    char* redirectionWrite = ">";
    char* redirectionRead = "<";
    char* redirectionDoubleWrite = ">>"; 
    size_t strBegin = 0;
    size_t strEnd = 0;

    for (size_t i = 0; i < length; i++) {
    	//Le début d'un token
        if (cmd[i] != ' ') { 
            strBegin = i;
            strEnd = strBegin + 1;
            if (cmd[i] == '\'') {
                simpleQuoteMode = !simpleQuoteMode;
            } else if (cmd[i] == '\"') {
                doubleQuoteMode = !doubleQuoteMode;
            } 
            
            while (strEnd < length && !isspace(cmd[strEnd])) {
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
		        }else{
		        	strEnd++;
		        }
           	}	
           	
            token = (char*) malloc((strEnd - strBegin) * sizeof(char));
            if (token == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }

            strncpy(token, cmd + strBegin, strEnd - strBegin);
            token[strEnd - strBegin] = '\0';

            tokenList = (char**) realloc(tokenList, (index + 1) * sizeof(char*));
            if (tokenList == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }

            // Check les redirections et les prendre en compte
			if (token[0] == '>' && token[1] == '>') {
				tokenList[index++] = strdup(redirectionDoubleWrite);
				if (token[2] != '\0') {
					char* restOfToken = strdup(token + 2); 
					tokenList = (char**) realloc(tokenList, (index + 2) * sizeof(char*));
					if (tokenList == NULL) {
						fprintf(stderr, "Memory allocation failed\n");
						exit(EXIT_FAILURE);
					}
					tokenList[index++] = restOfToken; 
				}
			} else if (token[0] == '>' && token[1] != '\0') {
				tokenList[index++] = strdup(redirectionWrite);
				if (token[1] != '\0') {
					char* restOfToken = strdup(token + 1); 
					tokenList = (char**) realloc(tokenList, (index + 2) * sizeof(char*));
					if (tokenList == NULL) {
						fprintf(stderr, "Memory allocation failed\n");
						exit(EXIT_FAILURE);
					}
					tokenList[index++] = restOfToken; 
				}
			} else if (token[0] == '<' && token[1] != '\0') {
				tokenList[index++] = strdup(redirectionRead);
				if (token[1] != '\0') {
					char* restOfToken = strdup(token + 1); 
					tokenList = (char**) realloc(tokenList, (index + 2) * sizeof(char*));
					if (tokenList == NULL) {
						fprintf(stderr, "Memory allocation failed\n");
						exit(EXIT_FAILURE);
					}
					tokenList[index++] = restOfToken; 
				}
			} else {
				tokenList[index++] = strdup(interpretQuotes(strdup(token)));
			}

            i = strEnd - 1; 
        }
    }

    // Ajoute NULL pour marquer la fin du tableau
    tokenList = (char**) realloc(tokenList, (index + 1) * sizeof(char*));
    if (tokenList == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    tokenList[index] = NULL;
    
    printf("\n");
    for(int i = 0; tokenList[i] != NULL; i++){
    	printf("%s ", tokenList[i]);
    }
    printf("\n");
    
    return tokenList;
}
