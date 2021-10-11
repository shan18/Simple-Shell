#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "cmd.h"

// Function declaration for builtin-commands
int builtin_cd(char **args);
int builtin_exit();

char *builtin_str[] = {
    "cd",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &builtin_cd,
    &builtin_exit,
};

int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int builtin_cd(char **args) {
    int count;
    for(count = 0; args[count] != NULL; count++);
    if(count != 2) {
        fprintf(stderr, "Error: invalid command\n");
    } else {
        if(chdir(args[1]) == -1) fprintf(stderr, "Error: invalid directory\n");
    }
    return 1;
}

int builtin_exit() {
    return 0;
}

void execute_child(char **args) {
    char *envp[] = {"PATH=/bin/", 0};
    char abs_cmd[100] = "/bin/";
    strcat(abs_cmd, args[0]);
    if(execve(abs_cmd, args, envp) == -1) {
        envp[0] = "PATH=/usr/bin/";
        strcpy(abs_cmd, "/usr/bin/");
        strcat(abs_cmd, args[0]);
        if(execve(abs_cmd, args, envp) == -1) {
            fprintf(stderr, "Error: invalid program\n");
            exit(-1);
        }
    }
}

int run_external(char **args) {
    int status;
    if(fork()) {
        waitpid(-1, &status, 0);
    } else {
        execute_child(args);
    }
    return 1;
}

int run_cmd(char **args) {
    // Check builtin commands
    for (int i = 0; i < num_builtins(); i++) {
        if(strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    
    return run_external(args);
    
    // if (*args[0] == '/' || strstr(args[0], "./") == args[0]) {
    //     return;
    // } else if() {
    //     return;
    // } else if(strcmp(args[0], "ls") == 0) {
    //     run_external(args);
    // }
}
