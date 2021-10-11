/* References
    - https://brennan.io/2015/01/16/write-a-shell-in-c/
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "init.h"

#define MAX_INPUT_BUFFER_SIZE 1001
#define EXIT_SUCCESS 0
#define EXIT_FALIURE -1

int main() {
    char **args = malloc(((MAX_INPUT_BUFFER_SIZE / 2) + 1) * sizeof(char*));
    if(!args) {
        fprintf(stderr, "Error: Cannot allocate memory to tokenize inputs. Exiting!\n");
    }

    do {
        get_shell_input(args, MAX_INPUT_BUFFER_SIZE);
    } while (strcmp(args[0], "exit") != 0);

    // Free memory
    free_args(args, NULL);

    return EXIT_SUCCESS;
}
