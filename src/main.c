#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include "hashTable.h"
#include "execution.h"
#include "parsing.h"
#include "builtIn.h"

int main(int argc, char *argv[]) 
{
    char* buffer = NULL;
    size_t buffer_size = 1000;
    HashTable* table_env = create_table(10);
    Env_variable* var_env = NULL;
    char** buffer_parsed = NULL;
    
    extern char **environ;
    char **env = environ;
    
    while (*env) {
    	var_env = split(*env);
        insert(table_env, var_env->first_part, var_env->second_part);
        env++;
    }
    
    //printf("%s", search(table_env, "PATH"));

    // Allocation du tampon pour stocker la commande entrée par l'utilisateur
    buffer = (char *)malloc(buffer_size * sizeof(char));
    if (buffer == NULL) 
    {
        perror("Malloc failure");
        return (EXIT_FAILURE);
    }

    // Écrire un invite de commande
    write(STDOUT_FILENO, "$> ", 3);

    // Lire depuis STDIN dans une boucle
    while (getline(&buffer, &buffer_size, stdin) != -1) 
    {
        // Supprimer le saut de ligne final
        buffer[strcspn(buffer, "\n")] = '\0';
        //parsing
        buffer_parsed = parse(buffer);
        if(is_built_in(buffer_parsed[0]))
        {
        	exec_buit_in(buffer_parsed);
        }
        else
        {
        	exec_cmd(parse(buffer), search(table_env, "PATH"));
        }	
        
        // Écrire un autre invite de commande
        write(STDOUT_FILENO, "$> ", 3);
    }

    // Libérer le tampon alloué
    free(buffer);

    return 0;
}

