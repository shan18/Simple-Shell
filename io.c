#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "io.h"
#include "validator.h"

int replace_stdio(char *std_fn, char *stdtype) {
    int stdio_fd, io_redirect;
    if(strcmp(stdtype, "stdout_t") == 0 || strcmp(stdtype, "stdout_a") == 0) {
        stdio_fd = dup(STDOUT_FILENO);
        io_redirect = stdtype[strlen(stdtype) - 1] == 't' ? open(std_fn, O_CREAT | O_TRUNC | O_WRONLY) : open(std_fn, O_CREAT | O_APPEND | O_WRONLY);
        dup2(io_redirect, STDOUT_FILENO);
        dup2(io_redirect, STDERR_FILENO);  // Not required
    } else if(strcmp(stdtype, "stdin") == 0) {
        stdio_fd = dup(STDIN_FILENO);
        if ((io_redirect = open(std_fn, O_RDONLY)) == -1) {
            show_invalid_msg("invalid file");
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
