#include <stdio.h>

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
