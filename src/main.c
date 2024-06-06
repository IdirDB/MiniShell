#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "hashTable.h"
#include "execution.h"
#include "parsing.h"
#include "builtIn.h"

// Function Prototypes
void initialize_environment(HashTable *table);
void prompt();
void handle_input(char *buffer, size_t buffer_size, HashTable *table_env);
char** splitCmdWithRedirection(char** cmd, int index);
int thereIsRedirection(char** command_parsed);
void executeCmd(char** cmd, HashTable *table_env);

/**
 * @brief Main function to run the command-line interface.
 * 
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return int Exit status of the program.
 */
int main(int argc, char *argv[]) {
    char *buffer = NULL;
    size_t buffer_size = 1000;
    HashTable *table_env = create_table(10);

    // Initialize environment variables in the hash table
    initialize_environment(table_env);

    // Allocate buffer for user input
    buffer = (char *)malloc(buffer_size * sizeof(char));
    if (buffer == NULL) {
        perror("Malloc failure");
        free_table(table_env);
        return EXIT_FAILURE;
    }

    // Command prompt loop
    prompt();
    while (getline(&buffer, &buffer_size, stdin) != -1) {
        // Handle user input
        handle_input(buffer, buffer_size, table_env);
        prompt();
    }

    // Free allocated resources
    free(buffer);
    free_table(table_env);

    return 0;
}

/**
 * @brief Initializes the hash table with environment variables.
 * 
 * @param table The hash table to populate with environment variables.
 */
void initialize_environment(HashTable *table) {
    extern char **environ;
    char **env = environ;
    Env_variable *var_env = NULL;

    while (*env) {
        var_env = split(*env);
        if (var_env) {
            insert(table, var_env->first_part, var_env->second_part);
            free(var_env->first_part);
            free(var_env->second_part);
            free(var_env);
        }
        env++;
    }
}

/**
 * @brief Displays the command prompt.
 */
void prompt() {
    write(STDOUT_FILENO, "$> ", 3);
}

/**
 * @brief Handles the input from the user.
 * 
 * @param buffer The input buffer.
 * @param buffer_size The size of the input buffer.
 * @param table_env The hash table containing environment variables.
 */
void handle_input(char *buffer, size_t buffer_size, HashTable *table_env) {
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline character
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);

    // Parse the input buffer into a list of commands
    char **buffer_parsed = parseLigne(buffer);
    if (buffer_parsed == NULL) 
    	exit(EXIT_FAILURE);

    for (int i = 0; buffer_parsed[i] != NULL; i++) { 
    	int file = -1;  
        char** command_parsed = parseCmd(buffer_parsed[i]);
        if (command_parsed == NULL){
        	perror("Error parseCmd");
        	exit(EXIT_FAILURE);
        }	
		int indexFirstRedirection = thereIsRedirection(command_parsed);
		if(indexFirstRedirection > 0){
			for(int i = 0; command_parsed[i] != NULL; i++){
				if(!strcmp(command_parsed[i], ">" )){
					file = open(command_parsed[i+1], O_WRONLY | O_CREAT| O_TRUNC, 0777);
					dup2(file, STDOUT_FILENO);
					close(file);
				}else if(!strcmp(command_parsed[i], ">>")){
					file = open(command_parsed[i+1], O_WRONLY | O_CREAT| O_APPEND, 0777);
					dup2(file, STDOUT_FILENO);
					close(file); 
				}else if(!strcmp(command_parsed[i], "<")){
					file = open(command_parsed[i+1], O_RDONLY, 0777);
					dup2(file, STDIN_FILENO);
					close(file);
				}	
			}
			char** cmd = splitCmdWithRedirection(command_parsed, indexFirstRedirection);
			executeCmd(cmd, table_env);
			dup2(saved_stdin, STDIN_FILENO);
			dup2(saved_stdout, STDOUT_FILENO);
		}else{
			executeCmd(command_parsed, table_env);
		}
    }
    free(buffer_parsed); 
}

void executeCmd(char** cmd, HashTable *table_env){
	if (is_built_in(cmd[0])) {
		exec_built_in(cmd);
	} else {
		exec_cmd(cmd, search(table_env, "PATH"));
	}
}	

int thereIsRedirection(char** command_parsed){
	for(int i = 0; command_parsed[i] != NULL; i++){
		if(!strcmp(command_parsed[i], ">") || !strcmp(command_parsed[i], ">>") || !strcmp(command_parsed[i], "<"))
			return i;
	}
	return -1;
}

char** splitCmdWithRedirection(char** cmd, int index){
	if(index <= 0){
		perror("Invalid index");
		exit(EXIT_FAILURE);
	}
	
	char** newCmd = (char**)malloc(index * sizeof(char*));
	if(newCmd == NULL){
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	
	for(int i = 0; i < index; i++){
		newCmd[i] = cmd[i];
	}	
	return newCmd;
}		





