#ifndef REDIRECTION_H
#define REDIRECTION_H

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

int thereIsRedirection(char** commandParsed);
void redirect(char** command_parsed);
char** splitCmdWithRedirection(char** cmd, int index);

#endif
