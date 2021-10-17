#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "cmd.h"
#include "init.h"
#include "io.h"
#include "validator.h"

#define MAX_INPUT_BUFFER_SIZE 1000

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
    char abs_cmd[100] = "/bin/";  // TODO: See this 100 size
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

int run_cmd(char line_input[]) {
    char *args[MAX_INPUT_BUFFER_SIZE];
    tokenize(line_input, " ", args);
    
    // free(line_input);
    if(!validate_args(args))
        return 1;

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

int run_pipe_cmd(char line_input[]) {
    int pc = get_pipe_count(line_input);
    int pids[pc + 1], fd[pc][2];
    char *cmds[pc + 2], *args[(MAX_INPUT_BUFFER_SIZE / 2) + 1];

    // Tokenize with pipe
    tokenize(line_input, "|", cmds);

    for(int i = 0; cmds[i] != NULL; i++) {
        tokenize(cmds[i], " ", args);
        if(!validate_args(args) || !validate_pipe_args(args, i, pc + 1) || pipe(fd[i]) == -1) {
            for(int j = 0; j < i; j++) {
                close(fd[j][0]);
                close(fd[j][1]);
            }
            return 1;
        }
        pids[i] = fork();
        if (pids[i] < 0) {
            for(int j = 0; j <= i && j < pc; j++) {
                close(fd[j][0]);
                close(fd[j][1]);
            }
            for(int j = 0; j < i; j++)
                waitpid(pids[j], NULL, 0);
            return 0;
        }
        if (pids[i] == 0) {
            if (i > 0)
                dup2(fd[i - 1][0], STDIN_FILENO);
            if (i < pc)
                dup2(fd[i][1], STDOUT_FILENO);
            for(int j = 0; j <= i && j < pc; j++) {
                close(fd[j][0]);
                close(fd[j][1]);
            }
            execute_child(args);
        }
        clear_buffer(args);
    }

    for(int i = 0; i < pc; i++) {
        close(fd[i][0]);
        close(fd[i][1]);
    }

    for(int i = 0; i < pc + 1; i++)
        waitpid(pids[i], NULL, 0);
    
    return 1;
}
