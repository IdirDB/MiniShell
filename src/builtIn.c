#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "builtIn.h"

extern char **environ; 

/**
 * @brief Checks if a command is a built-in shell command.
 * 
 * @param cmd The command to check.
 * @return int 1 if the command is a built-in command, 0 otherwise.
 */
int is_built_in(const char* cmd) {
    const char* build_in[] = {"cd", "pwd", "echo", "exit", "env", "export", NULL};

    if (cmd == NULL) {
        return 0;
    }

    for (int i = 0; build_in[i]; i++) {
        if (strcmp(build_in[i], cmd) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Changes the current working directory to the specified path.
 * 
 * @param next_path The path to change the current working directory to.
 */
void built_in_cd(const char* next_path) {
    if (next_path == NULL) {
        fprintf(stderr, "Error: Null path provided to built_in_cd.\n");
        return;
    }

    if (chdir(next_path) == -1) {
        perror("chdir");
    }
}

/**
 * @brief Prints the current working directory to the standard output.
 */
void built_in_pwd(void) {
    char cwd[MAX_PATH_LEN];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd");
    }
}

/**
 * @brief Prints the specified string to the standard output.
 * 
 * @param str The string to print. If NULL, prints an error message.
 */
void built_in_echo(const char* str) {
    if (str == NULL) {
        fprintf(stderr, "Error: Null string provided to built_in_echo.\n");
        return;
    }
    printf("%s\n", str);
}

/**
 * @brief Exits the program with the specified status.
 * 
 * @param status_str The exit status as a string. If NULL or invalid, exits with status 0.
 */
void built_in_exit(const char* status_str) {
    int exit_status = 0;

    if (status_str != NULL) {
        char *endptr;
        exit_status = strtol(status_str, &endptr, 10);

        if (*endptr != '\0') {
            fprintf(stderr, "Error: Invalid exit status '%s'. Exiting with status 0.\n", status_str);
            exit_status = 0;
        }
    }
    exit(exit_status);
}

void built_in_env(){
	for (int i = 0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }
}

void built_in_export(char** cmd) {
    char* arg = cmd[1];
    
    if (arg == NULL) {
        // Afficher toutes les variables d'environnement si aucun argument n'est fourni
        for (int i = 0; environ[i] != NULL; i++) {
            printf("declare -x %s\n", environ[i]);
        }
        return;
    } else if (arg[0] == '=' && arg[1] == ' ') {
        // Message d'erreur si l'argument commence par `= `
        fprintf(stderr, "export: `=': not a valid identifier\n");
    } else {
        // Diviser l'argument en nom et valeur de variable d'environnement
        char* delim = "=";
        char* envVariableName = strtok(arg, delim);
        char* envVariableValue = strtok(NULL, delim);
        
        
        if (envVariableName == NULL || envVariableValue == NULL) {
            fprintf(stderr, "export: Invalid argument format\n");
            return;
        }
        
        if (setenv(envVariableName, envVariableValue, 1) != 0) {
            perror("setenv");
            fprintf(stderr, "export: Failed to set environment variable\n");
    	}
	}	
}	


/**
 * @brief Executes a built-in shell command.
 * 
 * @param built_in The command and its arguments as an array of strings.
 */
void exec_built_in(char **built_in) {
    if (built_in == NULL || built_in[0] == NULL) {
        fprintf(stderr, "Error: Null command provided to exec_built_in.\n");
        return;
    }

    if (strcmp(built_in[0], "pwd") == 0) {
        built_in_pwd();
    } else if (strcmp(built_in[0], "cd") == 0) {
        built_in_cd(built_in[1]);
    } else if (strcmp(built_in[0], "echo") == 0) {
        built_in_echo(built_in[1]);
    } else if (strcmp(built_in[0], "exit") == 0) {
        built_in_exit(built_in[1]);
    } else if (strcmp(built_in[0], "env") == 0) {
    	built_in_env();
    } else if (strcmp(built_in[0], "export") == 0) {
    	built_in_export(built_in);
    } else {
        fprintf(stderr, "Error: Unknown built-in command '%s'.\n", built_in[0]);
    }
}

