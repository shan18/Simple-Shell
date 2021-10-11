#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "cmd.h"

void run_cmd(char **args) {
    if (*args[0] == '/') {
        return;
    } else if(strstr(args[0], "./") == args[0]) {
        return;
    } else if(strcmp(args[0], "ls") == 0) {
        int status;
        if (fork()) {
            waitpid(-1, &status, 0);
        } else {
            char *envp[] = {"PATH=/bin/", 0};
            char ee[] = "/bin/";
            strcat(ee, args[0]);
            if(execve(ee, args, envp) == -1) {
                fprintf(stderr, "Error: Invalid program\n");
            }
        }
    }
}
