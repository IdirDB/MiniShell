#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "execution.h"
#include "environ.h"

/**
 * @brief Exécute une commande externe avec une création de processus en utilisant fork().
 *
 * Cette fonction crée un processus enfant à l'aide de fork() pour exécuter une commande externe spécifiée
 * par "cmd". Elle utilise "execve" pour remplacer l'image du processus enfant avec celui du programme
 * spécifié par "full_path", "cmd" et environ.
 *
 * @param cmd Tableau de chaînes de caractères représentant la commande à exécuter et ses arguments.
 * @param path Chemin d'accès pour rechercher le programme à exécuter.
 *
 * @note execCmdWithFork utilise la fonction "rebuild_path" pour reconstruit le chemin complet d'un programme exécutable à partir de son nom de 
 * de commande.
 */
void 
execCmdWithFork(char **cmd, const char* path) {
    pid_t pid;

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } 
    
    if (pid == 0) {
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
        int status;
        wait(&status);
    }
}

/**
 * @brief Reconstruit le chemin complet d'un programme exécutable à partir de son nom de commande.
 *
 * Cette fonction recherche le programme "cmd"` dans les répertoires listés dans "path_env".
 * Si "path_env" est NULL, elle affiche une erreur.
 * Elle gère également les cas où "cmd" commence par '/' ou '.' pour spécifier un chemin absolu ou relatif.
 *
 * @param cmd Le nom de la commande ou le chemin relatif/absolu du programme.
 * @param path_env La chaîne de caractères représentant la variable d'environnement PATH.
 *
 * @return Le chemin complet de l'exécutable trouvé, ou NULL en cas d'erreur ou si le programme n'est pas trouvé.
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

    fprintf(stderr, "Command '%s' not found in PATH.\n", cmd);
    free(path_copy);
    return NULL;
}

/**
 * @brief Exécute une commande.
 *
 * Cette fonction détermine si la commande spécifiée par (cmd[0]) est une commande intégrée
 * ou une commande externe. Si c'est une commande intégrée, elle est exécutée directement
 * avec "execBuiltIn". Sinon, elle est exécutée en créant un nouveau processus avec "execCmdWithFork".
 *
 * @param cmd Tableau de chaînes de caractères représentant la commande à exécuter et ses arguments.
 *
 * @note Cette fonction utilise la fonction "isBuiltIn" pour vérifier si la commande est intégrée.
 */
void 
executeCmd(char** cmd){
	if (isBuiltIn(cmd[0])) {
		execBuiltIn(cmd);
	} else {
		execCmdWithFork(cmd, getenv("PATH"));
	}
}

