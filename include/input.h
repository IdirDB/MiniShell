#ifndef INPUT_H
#define INPUT_H

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

void handle_input(char *buffer, size_t buffer_size, char *envp[]);

#endif
