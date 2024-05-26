typedef struct Env_variable {
	char* first_part;
	char* second_part;
} Env_variable;

char** parse(char* raw_cmd);
void free_cmd(char* cmd[]);
Env_variable* split(const char* str);
