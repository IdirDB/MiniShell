void initialize_environment(HashTable *table);
void prompt();
void handle_input(char *buffer, size_t buffer_size, char *envp[]);
void redirect(char** command_parsed);
void executeCmd(char** cmd);
int thereIsRedirection(char** command_parsed);
char** splitCmdWithRedirection(char** cmd, int index);

