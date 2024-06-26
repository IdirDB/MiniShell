#ifndef BUILTIN_H
#define BUILTIN_H

#include "hashTable.h"
#define MAX_PATH_LEN 2500

int is_built_in(const char* cmd);
void built_in_cd(const char* next_path);
void built_in_pwd(void);
void built_in_echo(const char* str);
void built_in_exit(const char* status_str);
void built_in_env();
void built_in_export(char** cmd);
void exec_built_in(char **built_in);


#endif
