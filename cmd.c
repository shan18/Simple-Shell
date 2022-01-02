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

char *SUSPENDED_JOBS[100];
int SUSPENDED_PIDS[100];
int SUSPENDED_JOB_COUNTER = 0;
char *CURRENT_JOB;
int CURRENT_PID = -1;

void signal_handler(int sigid) {
    if ((sigid == SIGTSTP || sigid == SIGSTOP) && CURRENT_PID >= 0) {
        SUSPENDED_JOBS[SUSPENDED_JOB_COUNTER] = CURRENT_JOB;
        SUSPENDED_PIDS[SUSPENDED_JOB_COUNTER] = CURRENT_PID;
        SUSPENDED_JOB_COUNTER++;
    }
    signal(sigid, signal_handler);
}

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
        show_invalid_msg("invalid directory");
    }
    return 1;
}

int builtin_exit(char *args[]) {
    if(validate_num_args(args, 1)) {
        SUSPENDED_JOB_COUNTER > 0 ? show_invalid_msg("there are suspended jobs") : exit(0);
    }
    return 1;
}

int builtin_fg(char *args[]) {
    if(validate_num_args(args, 2)) {
        int job_idx = atoi(args[1]) - 1;
        if (job_idx < 0 || job_idx >= SUSPENDED_JOB_COUNTER)
            show_invalid_msg("invalid job");
        else {
            int pid = SUSPENDED_PIDS[job_idx];
            char *job_name = SUSPENDED_JOBS[job_idx];
            for(int i = job_idx + 1; i < SUSPENDED_JOB_COUNTER; i++) {
                SUSPENDED_PIDS[i - 1] = SUSPENDED_PIDS[i];
                SUSPENDED_JOBS[i - 1] = strdup(SUSPENDED_JOBS[i]);
            }
            SUSPENDED_JOB_COUNTER--;
            kill(pid, SIGCONT);
            CURRENT_PID = pid;
            CURRENT_JOB = strdup(job_name);
            waitpid(pid, NULL, WUNTRACED);
            CURRENT_PID = -1;
        }
    }
    return 1;
}

int builtin_jobs(char *args[]) {
    if(validate_num_args(args, 1)) {
        for(int i = 0; i < SUSPENDED_JOB_COUNTER; i++) {
            printf("[%d] %s\n", i + 1, SUSPENDED_JOBS[i]);
        }
    }
    return 1;
}

void execute_child(char *args[]) {
    if(strstr(args[0], "./") == args[0]) {
        if(execv(args[0], args) == -1) {
            show_invalid_msg("invalid program");
            exit(-1);
        }
    } else {
        char *envp[] = {"PATH=/bin/", "TERM=xterm-256color", 0};
        char abs_cmd[MAX_INPUT_BUFFER_SIZE] = "/bin/";
        strcat(abs_cmd, args[0]);
        if(execve(abs_cmd, args, envp) == -1) {
            envp[0] = "PATH=/usr/bin/";
            strcpy(abs_cmd, "/usr/bin/");
            strcat(abs_cmd, args[0]);
            if(execve(abs_cmd, args, envp) == -1) {
                show_invalid_msg("invalid program");
                exit(-1);
            }
        }
    }
}

int run_external(char *args[]) {
    int pid = fork();
    if (pid < 0)
        return 1;

    CURRENT_PID = pid;

    if(pid == 0)
        execute_child(args);

    int child_status;
    waitpid(pid, &child_status, WUNTRACED);
    if (!WIFEXITED(child_status) && WIFSTOPPED(child_status)) {
        signal_handler(SIGTSTP);
    }

    CURRENT_PID = -1;
    return 1;
}

int run_cmd(char line_input[]) {
    CURRENT_JOB = strdup(line_input);

    char *args[MAX_INPUT_BUFFER_SIZE];
    tokenize(line_input, " ", args);

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
}

int run_pipe_cmd(char line_input[]) {
    CURRENT_JOB = strdup(line_input);

    if(!validate_pipe_line(line_input))
        return 1;

    int pc = get_pipe_count(line_input), pipes_opened = 0, processes_started = 0, is_valid_cmd = 1;
    int pids[pc + 1], fd[pc][2];
    char *cmds[pc + 2], *args[(MAX_INPUT_BUFFER_SIZE / 2) + 1];

    // Tokenize with pipe
    tokenize(line_input, "|", cmds);

    // Validate args before opening pipes
    for(int i = 0; cmds[i] != NULL && is_valid_cmd; i++) {
        tokenize(strdup(cmds[i]), " ", args);
        is_valid_cmd = validate_args(args) && validate_pipe_args(args, i, pc + 1) && validate_pipe_builtins(args[0], builtin_str, num_builtins());
        clear_buffer(args);
    }

    for(int i = 0; cmds[i] != NULL && is_valid_cmd; i++) {
        tokenize(cmds[i], " ", args);

        if(i < pc) {
            pipes_opened++;
            if(pipe(fd[i]) == -1)
                break;
        }

        // Check for io redirection
        int stdin_fd = input_redirection(args);
        int stdout_fd = output_redirection(args);

        if (stdin_fd == -2 || stdout_fd == -2)
            break;

        pids[i] = fork();
        if (pids[i] < 0)
            break;

        CURRENT_PID = pids[i];

        if (pids[i] > 0)
            processes_started++;
        else if (pids[i] == 0) {
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

        // Restore redirected io
        if (stdin_fd >= 0)
            restore_stdio(stdin_fd, "stdin");
        if (stdout_fd >= 0)
            restore_stdio(stdout_fd, "stdout");

        clear_buffer(args);
    }

    for(int i = 0; i < pipes_opened; i++) {
        close(fd[i][0]);
        close(fd[i][1]);
    }

    for(int i = 0; i < processes_started; i++)
        waitpid(pids[i], NULL, WUNTRACED);

    CURRENT_PID = -1;

    return 1;
}
