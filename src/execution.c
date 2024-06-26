#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "execution.h"

extern char **environ; 

/**
 * @brief Executes a command in a child process and waits for its completion in the parent process.
 * 
 * @param cmd The command and its arguments as an array of strings.
 * @param path The PATH environment variable.
 */
void exec_cmd(char **cmd, const char* path) {
    pid_t pid;

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } 
    
    if (pid == 0) {
        // Child process
        char *full_path = rebuild_path(cmd[0], path);
        if (full_path == NULL) {
            exit(EXIT_FAILURE);
        }
        
        if (execve(full_path, cmd, environ) == -1) {
            perror("execve");
            free(full_path);
            exit(EXIT_FAILURE);
        }
        
        free(full_path);
        exit(EXIT_SUCCESS);
        
    } else {
        // Parent process
        int status;
        // Attendre la fin du processus enfant
        wait(&status);
    }
}

/**
 * @brief Reconstructs the full path of a command by searching through the PATH environment variable.
 * 
 * @param cmd The command to find.
 * @param path_env The PATH environment variable.
 * @return char* The full path of the command if found, NULL otherwise.
 */
char* rebuild_path(const char *cmd, const char *path_env) {
    if (path_env == NULL) {
        fprintf(stderr, "The PATH environment variable is not set.\n");
        return NULL;
    }

    char *path_copy = strdup(path_env);
    if (path_copy == NULL) {
        fprintf(stderr, "strdup\n");
        return NULL;
    }
    
    if(cmd[0] == '/'){
    	return strdup(cmd);
    }else if(cmd[0] == '.'){
    	char current_dir[MAX_PATH_LEN];
    	char full_path[MAX_PATH_LEN];
    	if (getcwd(current_dir, sizeof(current_dir)) != NULL) {
    		snprintf(full_path, sizeof(full_path), "%s/%s", current_dir, cmd+2);
    		return strdup(full_path);
    	}else{
    		perror("getcwd");
        	exit(EXIT_FAILURE);
    	}
    }

    char *token = strtok(path_copy, ":");
    while (token != NULL) {
        char full_path[MAX_PATH_LEN];
        snprintf(full_path, sizeof(full_path), "%s/%s", token, cmd);

        if (access(full_path, X_OK) == 0) {
            // Executable file found
            free(path_copy);
            return strdup(full_path);
        }

        token = strtok(NULL, ":");
    }

    // Command not found in any directories of PATH
    fprintf(stderr, "Command '%s' not found in PATH.\n", cmd);
    free(path_copy);
    return NULL;
}

