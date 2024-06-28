#ifndef BUILTIN_H
#define BUILTIN_H

#include "hashTable.h"

#define MAX_PATH_LEN 2500

int isBuiltIn(const char* cmd);
void builtInCd(const char* next_path);
void builtInPwd();
void builtInEcho(char** cmd);
void builtInExit(const char* status_str);
void builtInEnv();
void builtInExport(char** cmd);
void builtInUnset(const char* varName);
void execBuiltIn(char **built_in);

#endif
