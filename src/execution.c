#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "execution.h"

void exec_cmd(char **cmd, char* path)
{
    pid_t pid = 0;
    int status = 0;

    pid = fork();
    if(pid == -1)
        perror("fork");
    else if(pid > 0)
    {
        // On bloque le processus parent jusqu'à ce que l'enfant termine puis
        // on tue le processus enfant
        waitpid(pid, &status, 0);
        kill(pid, SIGTERM);
    }
    else{
        //int execve(const char *pathname, char *const argv[], char *const envp[]);
        if(execve(rebuild_path(cmd[0], path), cmd, NULL) == -1){
            perror("execve : ");
            exit(EXIT_FAILURE);
        }   
    }
}

// Fonction pour reconstruire le chemin complet d'une commande
char* rebuild_path(const char *cmd, const char *path_env) {
    if (path_env == NULL) {
        fprintf(stderr, "La variable d'environnement PATH n'est pas définie.\n");
        return NULL;
    }

    char *path_copy = strdup(path_env);
    if (path_copy == NULL) {
        perror("Erreur de duplication de la variable PATH");
        return NULL;
    }

    char *token = strtok(path_copy, ":");
    while (token != NULL) {
        char full_path[MAX_PATH_LEN];
        snprintf(full_path, sizeof(full_path), "%s/%s", token, cmd);

        if (access(full_path, X_OK) == 0) {
            // Le fichier exécutable a été trouvé
            return strdup(full_path); // Retourner le chemin trouvé
        }

        token = strtok(NULL, ":");
    }

    // La commande n'a pas été trouvée dans les répertoires de PATH
    fprintf(stderr, "La commande %s n'a pas été trouvée.\n", cmd);
    free(path_copy);
    return NULL;
}
