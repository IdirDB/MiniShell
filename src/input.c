#include "input.h"

/**
 * @brief Gère l'exécution des commandes à partir d'une entrée utilisateur.
 *
 * Cette fonction prend une chaîne de caractères "buffer" représentant une entrée utilisateur,
 * la divise en commandes individuelles, puis exécute chaque commande. Elle gère les pipes entre
 * les commandes et redirige l'entrée/sortie standard selon les spécifications des commandes.
 *
 * @param buffer Le buffer contenant l'entrée utilisateur à traiter.
 * @param buffer_size La taille du buffer "buffer".
 * @param envp Le tableau d'environnement (variable d'environnement) pour les commandes.
 *
 * @note La fonction utilise : 
 *		 "parseLigne" pour diviser "buffer" en commandes
 *		 Exemple : 
 *		 buffer = "cmd1 | cmd2; cmd3" -> bufferParsed = ["cmd1 | cmd2", "cmd3"].
 *
 *       "parseCmdPipe" pour gérer les commandes séparées par des pipes '|'
 *		 Exemple :
 *		 bufferParsed = ["cmd1 | cmd2", "cmd3"] -> commandParsed = [linkedList(["cmd1"] -> ["cmd2"] -> [NULL]), linkedList(["cmd3"] -> [NULL])]
 *		
 *       "parseCmd" pour découper chaque commande en tokens
 *		 Exemple :
 *		 Si cmd1 = echo "Hello" "World" -> cmdParsed = ["echo", "Hello", "World", NULL];
 *
 *		"thereIsRedirection" pour détecter les redirections. 
 *		Exemple :
 *		Si cmdParsed = ["echo", "Hello", "World", NULL] -> 0
 *		Si cmdParsed = ["echo", "Hello", ">", "a.txt", NULL] -> 1
 *
 *		"redirect" pour effectuer les redirections.
 *		
 *		"splitCmdWithRedirection" pour séparer la commande principale des arguments de redirection.
 *		Exemple :
 *		Si cmdParsed = ["echo", "Hello", ">", "a.txt", NULL] -> cmdParsed = ["echo", "Hello"]
 *
 * 		"executeCmd" pour exécuter chaque commande avec ses arguments.	
 */
 
void 
handle_input(char *buffer, size_t buffer_size, char *envp[]) {
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);
    
    buffer[strcspn(buffer, "\n")] = '\0'; 

    char** buffer_parsed = parseLigne(buffer);
    
    if (buffer_parsed == NULL){ 
        exit(EXIT_FAILURE);
	}
	
    for (size_t i = 0; buffer_parsed[i] != NULL; i++) {
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
            
            int indexFirstRedirection = thereIsRedirection(command_parsed);        
            if (indexFirstRedirection > 0) {
                redirect(command_parsed);
                char** new_command_parsed = splitCmdWithRedirection(command_parsed, indexFirstRedirection);
                free(command_parsed);
                command_parsed = new_command_parsed;
            }

            executeCmd(command_parsed);
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
