#include <stdio.h>
#include <string.h>

#include "validator.h"

const char *TERMINALS[] = {"<", ">", ">>", NULL};

void show_invalid() {
    fprintf(stderr, "Error: invalid command\n");
}

void show_invalid_msg(char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
}

int validate_num_args(char *args[], int num) {
    int count;
    for (count = 0; args[count] != NULL; count++);
    if (count != num) {
        show_invalid();
        return 0;
    }
    return 1;
}

int detect_edge_str(char *args[], int args_len, char const *str) {
    if (strcmp(args[0], str) == 0 || strcmp(args[args_len - 1], str) == 0)
        return 1;
    return 0;
}

int detect_multi_count(char *args[], char const *str) {
    int flag = 0;
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], str) == 0) {
            if (flag)
                return 1;
            flag = 1;
        }
    }
    return 0;
}

int detect_single_count(char *args[], char const *str) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], str) == 0)
            return 1;
    }
    return 0;
}

int validate_terminal_grammar(char *args[], int idx, int args_len) {
    return idx + 2 == args_len || (strcmp(args[idx + 2], "<") == 0 && idx + 4 == args_len);
}

int validate_stdin_grammar(char *args[], int idx, int args_len) {
    return idx + 2 == args_len || strcmp(args[idx + 2], "|") == 0 || strcmp(args[idx + 2], ">") == 0 || strcmp(args[idx + 2], ">>") == 0;
}

int validate_args(char *args[]) {
    // Get args count
    int args_len;
    for (args_len = 0; args[args_len] != NULL; args_len++);

    // Validate terminate strings
    for (int i = 0; TERMINALS[i] != NULL; i++) {
        if (detect_edge_str(args, args_len, TERMINALS[i]) || detect_multi_count(args, TERMINALS[i])) {
            show_invalid();
            return 0;
        }
    }

    // Validate recursive strings and detect invalid strings
    if (detect_edge_str(args, args_len, "|") || detect_single_count(args, "<<")) {
        show_invalid();
        return 0;
    }

    // Validate grammar
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0) {
            if (!validate_terminal_grammar(args, i, args_len)) {
                show_invalid();
                return 0;
            }
        } else if (strcmp(args[i], "<") == 0) {
            if (!validate_stdin_grammar(args, i, args_len)) {
                show_invalid();
                return 0;
            }
        }
    }

    return 1;
}

int validate_pipe_line(char *line) {
    int start, end;
    
    // Check if line begins with |
    for (start = 0; line[start] == ' '; start++);
    if (line[start] == '|') {
        show_invalid();
        return 0;
    }

    // Check if line ends with |
    for (end = strlen(line) - 1; line[end] == ' '; end--);
    if (line[end] == '|') {
        show_invalid();
        return 0;
    }

    return 1;
}

int validate_pipe_args(char *args[], int idx, int max_len) {
    if (idx > 0 && detect_single_count(args, TERMINALS[0])) {
        show_invalid();
        return 0;
    }

    if (idx < max_len - 1 && (detect_single_count(args, TERMINALS[1]) || detect_single_count(args, TERMINALS[2]))) {
        show_invalid();
        return 0;
    }

    return 1;
}

int validate_pipe_builtins(char *cmd, char *builtins[], int num_builtins) {
    for (int i = 0; i < num_builtins; i++) {
        if (strcmp(cmd, builtins[i]) == 0) {
            show_invalid();
            return 0;
        }
    }
    return 1;
}
