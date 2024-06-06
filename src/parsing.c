#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsing.h"

/* Parsing : cmd1; cmd2; cmd3;
Example : >ls; cd idir; -> [["ls"], ["cd", "idir"]]
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

/* Parsing : cmd1 arg1 arg2 
Example : echo "idir" -> [["echo"], ["idir"]]
*/
char** parseCmd(char* raw_cmd) {
    char* token = NULL;
    char** cmd = NULL;
    size_t index = 0;
    char* delim = " "; // Split on spaces
    char* redirectionDroite = ">";
    char* redirectionGauche = "<";
    char* redirectionDouble = ">>";  
    
    token = strtok(raw_cmd, delim);
    
    while (token != NULL) {
        cmd = (char**) realloc(cmd, (index + 1) * sizeof(char*));   
        if (token[0] == '>' && token[1] == '>') {
            cmd[index] = strdup(redirectionDouble);
            if(token[2] != '\0'){
		        token = token + 2;
		        index++;
				cmd = (char**) realloc(cmd, (index + 1) * sizeof(char*));   
				cmd[index] = strdup(token);
			}	
        } else if (token[0] == '>' && token[1] != '\0') { 
        	cmd[index] = strdup(redirectionDroite);
            token = token + 1;
            index++;
			cmd = (char**) realloc(cmd, (index + 1) * sizeof(char*));   
			cmd[index] = strdup(token);
        } else if (token[0] == '<' && token[1] != '\0'){
            cmd[index] = strdup(redirectionGauche);
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
	
	for(int i = 0; cmd[i] != NULL; i++){
		printf("%s ", cmd[i]);
	}
	printf("\n");
	
	return cmd;
}	

Env_variable* split(const char* str) {
    int len_first_part = 0;

    // Calculate the length of the first part (before the '=' delimiter)
    while (str[len_first_part] != '=' && str[len_first_part] != '\0') {
        len_first_part++;
    }

    // Check if the '=' delimiter is found
    if (str[len_first_part] != '=') {
        fprintf(stderr, "Error: The '=' character was not found in the string.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the Env_variable structure
    Env_variable* env_variable = (Env_variable*) malloc(sizeof(Env_variable));
    if (env_variable == NULL) {
        fprintf(stderr, "Memory allocation failed for env_variable");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the first_part and second_part
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

    // Copy the first part of the string
    strncpy(env_variable->first_part, str, len_first_part);
    env_variable->first_part[len_first_part] = '\0';
    
    // Copy the second part of the string (after the '=' delimiter)
    strcpy(env_variable->second_part, str + len_first_part + 1);
    env_variable->second_part[len_second_part] = '\0'; 

    return env_variable;
}
	   

