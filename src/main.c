#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "hashTable.h"
#include "execution.h"
#include "parsing.h"
#include "builtIn.h"

void initialize_environment(HashTable *table);
void prompt();
void handle_input(char *buffer, size_t buffer_size, HashTable *table_env);
char** splitCmdWithRedirection(char** cmd, int index);
int thereIsRedirection(char** command_parsed);
void executeCmd(char** cmd, HashTable *table_env);
void redirect(char** command_parsed);

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

void handle_input(char *buffer, size_t buffer_size, HashTable *table_env) {
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove the newline character
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);

    // cmd1; cmd2; cmd3 -> ["cmd1", "cmd2", "cmd3"]
    char** buffer_parsed = parseLigne(buffer);
    if (buffer_parsed == NULL) 
        exit(EXIT_FAILURE);

    for (int i = 0; buffer_parsed[i] != NULL; i++) {
        Node* headNodeCmdWithPipe = parseCmdPipe(buffer_parsed[i]);
        if (headNodeCmdWithPipe == NULL){
            perror("Error parseCmd");
            free(buffer_parsed);
            exit(EXIT_FAILURE);
        }
        
        int first = 1;
        int inputFd = STDIN_FILENO;
        
        while (headNodeCmdWithPipe != NULL) {
            int pipeFd[2];
            if (headNodeCmdWithPipe->next != NULL) {
                if (pipe(pipeFd) == -1) {
                    perror("pipe");
                    free(buffer_parsed);
                    exit(EXIT_FAILURE);
                }
            }
            
            if (!first) {                    
                dup2(inputFd, STDIN_FILENO);
                close(inputFd);
            }
            
            if (headNodeCmdWithPipe->next != NULL) {    
                dup2(pipeFd[1], STDOUT_FILENO);
                close(pipeFd[1]);
            } else {
                dup2(saved_stdout, STDOUT_FILENO);
            }
            
            char** command_parsed = parseCmd(headNodeCmdWithPipe->cmd);
            if (command_parsed == NULL) {
                perror("Error parseCmd");
                free(buffer_parsed);
                exit(EXIT_FAILURE);
            }
            
            // If there is a redirection
            int indexFirstRedirection = thereIsRedirection(command_parsed);        
            if (indexFirstRedirection > 0) {
                redirect(command_parsed);
                char** new_command_parsed = splitCmdWithRedirection(command_parsed, indexFirstRedirection);
                free(command_parsed);
                command_parsed = new_command_parsed;
            }

            // Execute command
            executeCmd(command_parsed, table_env);
            free(command_parsed);
            
            if (headNodeCmdWithPipe->next != NULL) {
                inputFd = pipeFd[0];
            }
            
            headNodeCmdWithPipe = headNodeCmdWithPipe->next;
            if (first) {
                first = 0;
            }
        }
        
        dup2(saved_stdin, STDIN_FILENO);
        dup2(saved_stdout, STDOUT_FILENO);
    }

    free(buffer_parsed); 
}

	
void redirect(char** command_parsed) {
    int file = 0;
    for (int k = 0; command_parsed[k] != NULL; k++) {
        if (!strcmp(command_parsed[k], ">")) {
            file = open(command_parsed[k+1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
            if (file < 0) {
                perror("open");
                return;
            }
            dup2(file, STDOUT_FILENO);
            close(file);
        } else if (!strcmp(command_parsed[k], ">>")) {
            file = open(command_parsed[k+1], O_WRONLY | O_CREAT | O_APPEND, 0777);
            if (file < 0) {
                perror("open");
                return;
            }
            dup2(file, STDOUT_FILENO);
            close(file);
        } else if (!strcmp(command_parsed[k], "<")) {
            file = open(command_parsed[k+1], O_RDONLY, 0777);
            if (file < 0) {
                perror("open");
                return;
            }
            dup2(file, STDIN_FILENO);
            close(file);
        }
    }
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

