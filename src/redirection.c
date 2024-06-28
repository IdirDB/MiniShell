#include "redirection.h"

/**
 * @brief Vérifie la présence de redirection dans un tableau de token.
 *
 * Cette fonction parcourt le tableau de token pour déterminer s'il y a un opérateur de redirection présent. 
 *
 * @param commandParsed Un tableau de token terminé par NULL.
 * @return L'index de l'opérateur de redirection s'il est trouvé, ou -1 s'il n'y a aucun opérateur de redirection.
 */
int 
thereIsRedirection(char** commandParsed){
	for(int i = 0; commandParsed[i] != NULL; i++){
		if(!strcmp(commandParsed[i], ">") || !strcmp(commandParsed[i], ">>") || !strcmp(commandParsed[i], "<"))
			return i;
	}
	return -1;
}

/**
 * Redirige l'entrée/sortie d'une commande.
 *
 * @param command_parsed Un tableau de token se terminant par NULL. Ce tableau inclut les 
 *						 symboles de redirection ('>', '>>', '<') et les noms de fichiers.
 */
void 
redirect(char** command_parsed) {
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

/**
 * Sépare une commande en deux tableaux de token : la commande principale et les redirections.
 *
 * @param cmd Le tableau original de tokens représentant la commande complète.
 * @param index L'index jusqu'où copier les tokens du tableau original pour la commande principale.
 * @return Un tableau de token représentant la commande principale.
 */
 
char** 
splitCmdWithRedirection(char** cmd, int index){
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
