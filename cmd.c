#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "cmd.h"
#include "io.h"
#include "validator.h"

// Function declaration for builtin-commands
int builtin_cd(char *args[]);
int builtin_exit(char *args[]);
int builtin_fg(char *args[]);
int builtin_jobs(char *args[]);

char *builtin_str[] = {
    "cd",
    "exit",
    "fg",
    "jobs"
};

int (*builtin_func[]) (char *[]) = {
    &builtin_cd,
    &builtin_exit,
    &builtin_fg,
    &builtin_jobs
};

int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int builtin_cd(char *args[]) {
    if(validate_num_args(args, 2) && chdir(args[1]) == -1) {
        show_invalid_msg("Error: invalid directory\n");
    }
    return 1;
}

int builtin_exit(char *args[]) {
    if(validate_num_args(args, 1)) return 0;
    return 1;
}

int builtin_fg(char *args[]) {
    if(validate_num_args(args, 2)) {
        return 1;
    }
    return 1;
}

int builtin_jobs(char *args[]) {
    if(validate_num_args(args, 1)) {
        return 1;
    }
    return 1;
}

void execute_child(char *args[]) {
    char *envp[] = {"PATH=/bin/", "TERM=xterm-256color", 0};
    char abs_cmd[100] = "/bin/";
    strcat(abs_cmd, args[0]);
    if(execve(abs_cmd, args, envp) == -1) {
        envp[0] = "PATH=/usr/bin/";
        strcpy(abs_cmd, "/usr/bin/");
        strcat(abs_cmd, args[0]);
        if(execve(abs_cmd, args, envp) == -1) {
            show_invalid();
            exit(-1);
        }
    }
}

int run_external(char *args[]) {
    int pid = fork();
    if(pid == 0)
        execute_child(args);
    waitpid(pid, NULL, 0);
    return 1;
}

int run_cmd(char *args[]) {
    // Run builtin commands
    for (int i = 0; i < num_builtins(); i++) {
        if(strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    // Check for io redirection
    int stdin_fd = input_redirection(args);
    int stdout_fd = output_redirection(args);
    
    // Run command
    int status = 1;
    if (stdin_fd != -2 && stdout_fd != -2) {
        status = run_external(args);
    }

    // Restore redirected io
    if (stdin_fd >= 0)
        restore_stdio(stdin_fd, "stdin");
    if (stdout_fd >= 0)
        restore_stdio(stdout_fd, "stdout");

    return status;
    
    // if (*args[0] == '/' || strstr(args[0], "./") == args[0]) {
    //     return;
}
