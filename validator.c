#include <stdio.h>
#include <string.h>

#include "validator.h"

void show_invalid() {
    fprintf(stderr, "Error: invalid command\n");
}

void show_invalid_msg(char *msg) {
    fprintf(stderr, "%s", msg);
}

int validate_num_args(char *args[], int num) {
    int count;
    for(count = 0; args[count] != NULL; count++);
    if (count != num) {
        show_invalid();
        return 0;
    }
    return 1;
}

int detect_edge_str(char *args[], int args_len, char *str) {
    if(strcmp(args[0], str) == 0 || strcmp(args[args_len - 1], str) == 0)
        return 1;
    return 0;
}

int detect_multi_count(char *args[], char *str) {
    int flag = 0;
    for(int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], str) == 0) {
            if (flag)
                return 1;
            flag = 1;
        }
    }
    return 0;
}

int detect_single_count(char *args[], char *str) {
    for(int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], str) == 0)
            return 1;
    }
    return 0;
}

int validate_args(char *args[]) {
    // Get args count
    int args_len;
    for(args_len = 0; args[args_len] != NULL; args_len++);

    // Validate terminate strings
    char *terminals[] = {"<", ">", ">>", NULL};
    for (int i = 0; terminals[i] != NULL; i++) {
        if(detect_edge_str(args, args_len, terminals[i]) || detect_multi_count(args, terminals[i])) {
            show_invalid();
            return 0;
        }
    }

    // Validate recursive strings and detect invalid strings
    if(detect_edge_str(args, args_len, "|") || detect_single_count(args, "<<")) {
        show_invalid();
        return 0;
    }

    return 1;
}
