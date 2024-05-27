#define MAX_PATH_LEN 2500

int is_built_in(char* cmd);
void built_in_cd(char* next_path);
void built_in_pwd(void);
void built_in_echo(char* str);
void built_in_exit(const char* status_str);
void exec_buit_in(char **built);
