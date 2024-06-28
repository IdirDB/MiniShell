#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "execution.h"
#include "parsing.h"
#include "builtIn.h"
#include "environ.h"
#include "redirection.h"
#include "input.h"
#include "affichage.h"

int 
main(int argc, char *argv[], char *envp[]) {
    char *buffer = NULL;
    size_t buffer_size = 1000;
		
    buffer = (char *)malloc(buffer_size * sizeof(char));
    if (buffer == NULL) {
        perror("Malloc failure");
        return EXIT_FAILURE;
    }

    prompt();
    while (getline(&buffer, &buffer_size, stdin) != -1) {
        handle_input(buffer, buffer_size, envp);
        prompt();
    }
    
    free(buffer);
    return 0;
}
