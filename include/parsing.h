#ifndef PARSING_H
#define PARSING_H

#include "hashTable.h"
#include "linkedList.h"

typedef struct Env_variable {
	char* first_part;
	char* second_part;
} Env_variable;

typedef struct Split_str {
	char* first_part;
	char* second_part;
} Split_str;

char** parseLigne(char* ligne);
char** parseCmd(char* cmd);
Node* parseCmdPipe(char* ligne);
void free_cmd(char* cmd[]);
Env_variable* split(const char* str);
int checkSimpleQuote(char* ligne);
int checkDoubleQuote(char* ligne);
char* strndup(const char* s, size_t n);
char* interpretQuotes(const char *input);

#endif
