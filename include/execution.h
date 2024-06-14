#define MAX_PATH_LEN 2500
#define MAX_CMD_LEN 256

void exec_cmd(char **cmd, const char* path);
char* rebuild_path(const char *cmd, const char *path_env);
void execPipeIn(char** cmd, const char* path, int* pipefd);
void execPipeOut(char** cmd, const char* path, int* pipefd);


