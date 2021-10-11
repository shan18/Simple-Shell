/* References
    - https://stackoverflow.com/a/60058438/6562425
    - https://www.geeksforgeeks.org/taking-string-input-space-c-3-different-methods/
    - https://www.tutorialspoint.com/cprogramming/c_error_handling.htm
    - https://stackoverflow.com/a/2328335/6562425
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <stdarg.h>

#include "init.h"

#define EXIT_SUCCESS 0
#define EXIT_FALIURE -1

void show_prompt() {
    char *buffer;
    if ((buffer = getcwd(NULL, 0)) != NULL) {
        printf("[nyush %s]$ ", basename(buffer));
    } else {
        fprintf(stderr, "Error: Failed to fetch current directory. Exiting!\n");
        exit(EXIT_FALIURE);
    }
}

void read_line(char *buffer, int max_buffer_size) {
    int buffer_counter = 0;
    int input_character = getchar();
    while(input_character != EOF && input_character != '\n') {
        if (buffer_counter < max_buffer_size - 1) {
            buffer[buffer_counter] = input_character;
            buffer_counter++;
        }
        input_character = getchar();
    }
    buffer[buffer_counter] = '\0';
}

int tokenize(char *line, char **args) {
    int position = 0;
    char *token = strtok_r(line, " ", &line);

    args[position] = malloc((strlen(token) + 1) * sizeof(char));
    strcpy(args[position], token);
    position++;

    while ((token = strtok_r(line, " ", &line))) {
        args[position] = malloc((strlen(token) + 1) * sizeof(char));
        strcpy(args[position], token);
        position++;
    }
    
    return position;
}

void fix_cmd_path(char *command) {
    if (command[0] != '/' && strchr(command, '/') != NULL && strstr(command, "./") != command) {
        size_t cmd_size = strlen(command);
        command = realloc(command, (cmd_size + 2) * sizeof(char));
        memmove(command + 2, command, cmd_size);
        memmove(command, "./", 2);
    }
}

int get_shell_input(char **args, int max_buffer_size) {
    char *line_input = malloc(sizeof(char) * max_buffer_size);
    if(!line_input) {
        fprintf(stderr, "Error: Cannot allocate buffer to read input. Exiting!\n");
        exit(EXIT_FALIURE);
    }

    show_prompt();
    read_line(line_input, max_buffer_size);
    int num_args = tokenize(line_input, args);
    fix_cmd_path(args[0]);

    free(line_input);

    return num_args;
}

void free_args(char **args, ...) {
    va_list ptr;
    va_start(ptr, args);

    while(args) {
        for(int i = 0; args[i] != NULL; ++i) {
            free(args[i]);
        }
        free(args);
        args = va_arg(ptr, char**);
    }

    va_end(ptr);
}
