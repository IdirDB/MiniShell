typedef struct Env_variable {
	char* first_part;
	char* second_part;
} Env_variable;

typedef struct Split_str {
	char* first_part;
	char* second_part;
} Split_str;

char** parseLigne(char* ligne);
char** parseCmd(char* raw_cmd);
void free_cmd(char* cmd[]);
Env_variable* split(const char* str);

