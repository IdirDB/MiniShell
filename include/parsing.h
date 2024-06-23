#include "linkedList.h"
#include "hashTable.h"

typedef struct Env_variable {
	char* first_part;
	char* second_part;
} Env_variable;

typedef struct Split_str {
	char* first_part;
	char* second_part;
} Split_str;

char** parseLigne(char* ligne);
char** parseCmd(char* cmd, HashTable *table_env);
Node* parseCmdPipe(char* ligne);
void free_cmd(char* cmd[]);
Env_variable* split(const char* str);
int checkSimpleQuote(char* ligne);
int checkDoubleQuote(char* ligne);
char* strndup(const char* s, size_t n);
char* interpretQuotes(const char *input, HashTable *table_env);


