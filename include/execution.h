#ifndef EXECUTION_H
#define EXECUTION_H

#define MAX_PATH_LEN 2500
#define MAX_CMD_LEN 256

#include "builtIn.h"

void executeCmdWithFork(const char** cmd);
void executeCmd(char** cmd);
char* rebuild_path(const char *cmd, const char *path_env);
void execPipeIn(char** cmd, const char* path, int* pipefd);
void execPipeOut(char** cmd, const char* path, int* pipefd);

#endif
