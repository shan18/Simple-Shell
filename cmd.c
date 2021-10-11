#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "cmd.h"

#define EXIT_SUCCESS 0
#define EXIT_FALIURE -1

void execute_child(char **args) {
    char *envp[] = {"PATH=/bin/", 0};
    char abs_cmd[100] = "/bin/";
    strcat(abs_cmd, args[0]);
    if(execve(abs_cmd, args, envp) == -1) {
        envp[0] = "PATH=/usr/bin/";
        strcpy(abs_cmd, "/usr/bin/");
        strcat(abs_cmd, args[0]);
        if(execve(abs_cmd, args, envp) == -1) {
            fprintf(stderr, "Error: Invalid program\n");
            exit(EXIT_FALIURE);
        }
    }
}

void run_external(char **args) {
    int status;
    if(fork()) {
        waitpid(-1, &status, 0);
    } else {
        execute_child(args);
    }
}

void run_cmd(char **args) {
    if (*args[0] == '/') {
        return;
    } else if(strstr(args[0], "./") == args[0]) {
        return;
    } else if(strcmp(args[0], "ls") == 0) {
        run_external(args);
    }
}
