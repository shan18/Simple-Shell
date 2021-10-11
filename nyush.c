#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "init.h"

#define EXIT_SUCCESS 0
#define EXIT_FALIURE -1
#define MAX_INPUT_BUFFER_SIZE 1001

int main() {
    int num_args;
    char **args = malloc(((MAX_INPUT_BUFFER_SIZE / 2) + 1) * sizeof(char*));
    if(!args) {
        fprintf(stderr, "Error: Cannot allocate memory to tokenize inputs. Exiting!\n");
    }

    do {
        num_args = get_shell_input(args, MAX_INPUT_BUFFER_SIZE);
        for (int i = 0; i < num_args; i++) {
            printf("%s\n", args[i]);
        }
    } while (strcmp(args[0], "exit") != 0);

    // Free memory
    free_args(args, NULL);

    return EXIT_SUCCESS;
}
