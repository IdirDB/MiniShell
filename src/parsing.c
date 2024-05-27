#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsing.h"

char** parse(char* raw_cmd)
{
    char* token = NULL;
    char** cmd = NULL;
    size_t index = 0;
    char* delim = " "; // Split sur les espaces
    
    token = strtok(raw_cmd, delim);
    
    while(token != NULL){
        cmd = (char**) realloc(cmd, (index + 1) * sizeof(char*));
        if (cmd == NULL) 
        {
            perror("Allocation de mémoire échouée");
            exit(EXIT_FAILURE);
        }
        cmd[index] = strdup(token);
        if (cmd[index] == NULL) 
        {
            perror("Allocation de mémoire échouée");
            exit(EXIT_FAILURE);
        }
        index++;
        token = strtok(NULL, delim);
    }
    // Ajout d'un pointeur NULL pour marquer la fin du tableau de chaînes
    cmd = (char**) realloc(cmd, (index + 1) * sizeof(char*));
    if (cmd == NULL) 
    {
        perror("Allocation de mémoire échouée");
        exit(EXIT_FAILURE);
    }
    cmd[index] = NULL;
    
    return cmd;
}
	
// Fonction pour diviser la chaîne en deux parties à partir du caractère '='
Env_variable* split(const char* str) 
{
    int len_first_part = 0;

    // Calculer la taille de la première partie (avant le délimiteur '=')
    while (str[len_first_part] != '=' && str[len_first_part] != '\0') 
    {
        len_first_part++;
    }

    // Vérifier si le délimiteur '=' est trouvé
    if (str[len_first_part] != '=') 
    {
        printf("Erreur: Le caractère '=' n'a pas été trouvé dans la chaîne.\n");
        exit(EXIT_FAILURE);
    }

    // Allouer de la mémoire pour la structure Env_variable
    Env_variable* env_variable = (Env_variable*) malloc(sizeof(Env_variable));
    if (env_variable == NULL) 
    {
        perror("Erreur d'allocation de mémoire pour env_variable");
        exit(EXIT_FAILURE);
    }

    // Allouer de la mémoire pour les parties first_part et second_part
    env_variable->first_part = (char*) malloc((len_first_part + 1) * sizeof(char)); 
    if (env_variable->first_part == NULL) 
    {
        perror("Erreur d'allocation de mémoire pour first_part");
        free(env_variable);
        exit(EXIT_FAILURE);
    }

    int len_second_part = strlen(str) - len_first_part - 1;
    env_variable->second_part = (char*) malloc((len_second_part + 1) * sizeof(char));
    if (env_variable->second_part == NULL) 
    {
        perror("Erreur d'allocation de mémoire pour second_part");
        free(env_variable->first_part);
        free(env_variable);
        exit(EXIT_FAILURE);
    }

    // Copier la première partie de la chaîne
    strncpy(env_variable->first_part, str, len_first_part);
    env_variable->first_part[len_first_part] = '\0';
	
    // Copier la seconde partie de la chaîne (après le délimiteur '=')
    strcpy(env_variable->second_part, str + len_first_part + 1);
    env_variable->second_part[len_second_part] = '\0'; 

    return env_variable;
}
