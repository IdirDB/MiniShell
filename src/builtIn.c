#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "builtIn.h"

int is_built_in(char* cmd){
	const char* build_in[] = {"cd", "pwd", "echo", "exit", NULL};
	
	for(int i = 0; build_in[i]; i++)
	{
		if(!strcmp(build_in[i], cmd))
			return 1;
	}
	return 0;	
}

void built_in_cd(char* next_path)
{
	if(chdir(next_path) == -1)
		perror("chdir()");
}	

void built_in_pwd(void)
{
	char cwd[MAX_PATH_LEN];
	
	if (getcwd(cwd, sizeof(cwd)) != NULL) 
	{
	       printf("%s\n", cwd);
	} else {
		perror("getcwd()");
	}
}

void built_in_echo(char* str) {
    if (str == NULL) {
        fprintf(stderr, "Error: Null string provided to build_in_echo.\n");
        return;
    }
    printf("%s\n", str);
}

void built_in_exit(const char* status_str) {
    int exit_status = 0;

    // Check if the status_str is not NULL and is a valid number
    if (status_str != NULL) 
    {
        // Try to convert the status_str to an integer
        char *endptr;
        exit_status = strtol(status_str, &endptr, 10);

        // If the conversion is not successful (not a valid number), exit with status 0
        if (*endptr != '\0') 
        {
            fprintf(stderr, "Error: Invalid exit status '%s'. Exiting with status 0.\n", status_str);
            exit_status = 0;
        }
    }
    // Exit the program with the specified status
    exit(exit_status);
}

void exec_buit_in(char **built_in){
		if (!strcmp(built_in[0], "pwd"))
			built_in_pwd();
		else if (!strcmp(built_in[0], "cd"))
			built_in_cd(built_in[1]);
		else if(!strcmp(built_in[0], "echo"))
			built_in_echo(built_in[1]);
		else if(!strcmp(built_in[0], "exit"))
			built_in_exit(built_in[1]);			
}
	
