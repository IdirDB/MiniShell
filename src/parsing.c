#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsing.h"

/* Parsing : cmd1; cmd2; cmd3;
Example : >ls | cat; cd idir; -> [["ls | cat"], ["cd idir"]]
*/
char** parseLigne(char* ligne){
    char* token = NULL;
    char** listCmd = NULL;
    size_t index = 0;
    char* delim = ";"; //parse on ";"
    
    token = strtok(ligne, delim);

    while(token != NULL){
        listCmd = (char**) realloc(listCmd, (index + 1) * sizeof(char*));
        if(listCmd == NULL){
            fprintf(stderr, "Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        listCmd[index] = strdup(token);
        if(listCmd[index] == NULL){
            fprintf(stderr, "Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        token = strtok(NULL, delim);
        index++;
    }
    
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
Node* parseCmdPipe(char* ligne){
    char* token = NULL;
    Node* headNode = NULL;
    char* delim = "|"; //parse on "|"
    
    token = strtok(ligne, delim);
	headNode = (Node*) malloc(sizeof(Node*));
	headNode->next = NULL;
	headNode->cmd = strdup(token);
    while(token != NULL){ 
		token = strtok(NULL, delim); 
		if(token != NULL)
			append(&headNode, strdup(token));  
    }    
    return headNode;
} 
   
/* Parsing : cmd1 arg1 arg2 
Example : [echo "idir"] -> [["echo"], ["idir"]]
*/
char** parseCmd(char* raw_cmd) {
    char* token = NULL;
    char** cmd = NULL;
    size_t index = 0;
    char* delim = " "; // Split on spaces and " "
    char* redirectionWrite = ">";
    char* redirectionRead = "<";
    char* redirectionDoubleWrite = ">>"; 
    
    token = strtok(raw_cmd, delim);
    
    while (token != NULL) {
        cmd = (char**) realloc(cmd, (index + 1) * sizeof(char*)); 
        if (token[0] == '>' && token[1] == '>') {
            cmd[index] = strdup(redirectionDoubleWrite);
            if(token[2] != '\0'){
		        token = token + 2;
		        index++;
				cmd = (char**) realloc(cmd, (index + 1) * sizeof(char*));   
				cmd[index] = strdup(token);
			}	
        } else if (token[0] == '>' && token[1] != '\0') { 
        	cmd[index] = strdup(redirectionWrite);
            token = token + 1;
            index++;
			cmd = (char**) realloc(cmd, (index + 1) * sizeof(char*));   
			cmd[index] = strdup(token);
        } else if (token[0] == '<' && token[1] != '\0'){
            cmd[index] = strdup(redirectionRead);
            token = token + 1;
            index++;
			cmd = (char**) realloc(cmd, (index + 1) * sizeof(char*));   
			cmd[index] = strdup(token);
        } else {
            cmd[index] = strdup(token); 
    	}
    	index++;
    	token = strtok(NULL, delim);
    }
    
	//Add NULL to the end
	cmd = (char**) realloc(cmd, (index + 1) * sizeof(char*));
	if (cmd == NULL) {
		fprintf(stderr, "Memory allocation failed 6\n");
		cmd[index] = NULL;
		exit(EXIT_FAILURE);
	}
	return cmd;
}

char* protection(char* token){

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
	   

