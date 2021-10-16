#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>

#include "cmd.h"
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
        puts("#### Yet to implement");
    }
    return 1;
}

int builtin_jobs(char *args[]) {
    if(validate_num_args(args, 1)) {
        puts("#### Yet to implement");
    }
    return 1;
}

void execute_child(char *args[]) {
    char *envp[] = {"PATH=/bin/", 0};
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
    int status;
    if(fork()) {
        waitpid(-1, &status, 0);
    } else {
        execute_child(args);
    }
    return 1;
}

int replace_stdio(char *std_fn, char *stdtype) {
    int stdio_fd, io_redirect;
    if(strcmp(stdtype, "stdout_t") == 0 || strcmp(stdtype, "stdout_a") == 0) {
        stdio_fd = dup(STDOUT_FILENO);
        io_redirect = stdtype[strlen(stdtype) - 1] == 't' ? open(std_fn, O_CREAT | O_TRUNC | O_WRONLY) : open(std_fn, O_CREAT | O_APPEND | O_WRONLY);
        dup2(io_redirect, STDOUT_FILENO);
        dup2(io_redirect, STDERR_FILENO);
    } else if(strcmp(stdtype, "stdin") == 0) {
        stdio_fd = dup(STDIN_FILENO);
        if ((io_redirect = open(std_fn, O_RDONLY)) == -1) {
            show_invalid_msg("Error: invalid file\n");
            return -2;
        }
        dup2(io_redirect, STDIN_FILENO);
    }
    close(io_redirect);
    return stdio_fd;
}

int update_io(char *args[], char *stdio_type, int fn_idx) {
    int stdio_fd = replace_stdio(args[fn_idx + 1], stdio_type);
    for(int j = fn_idx; args[j - 1] != NULL; j++)
        args[j] = args[j + 2];
    return stdio_fd;
}

int input_redirection(char *args[]) {
    for(int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0) {
            int stdin_fd = update_io(args, "stdin", i);
            return stdin_fd;
        }
    }
    return -1;
}

int output_redirection(char *args[]) {
    for(int i = 0; args[i] != NULL; i++) {
        int wr_t = strcmp(args[i], ">") == 0;
        int wr_a = strcmp(args[i], ">>") == 0;
        if (wr_t || wr_a) {
            if (i == 1) {
                show_invalid();
                return -2;
            }
            char *stdout_type = wr_t ? "stdout_t" : "stdout_a";
            int stdout_fd = update_io(args, stdout_type, i);
            return stdout_fd;
        }
    }
    return -1;
}

void restore_stdio(int stdio_fd, char *stdtype) {
    if(strcmp(stdtype, "stdout") == 0) {
        dup2(stdio_fd, STDOUT_FILENO);
    } else if(strcmp(stdtype, "stdin") == 0) {
        dup2(stdio_fd, STDIN_FILENO);
    }
    close(stdio_fd);
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
