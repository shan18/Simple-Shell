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

#define MAX_INPUT_BUFFER_SIZE 1001

void show_prompt() {
    char *buffer;
    if ((buffer = getcwd(NULL, 0)) != NULL) {
        printf("[nyush %s]$ ", basename(buffer));
    } else {
        fprintf(stderr, "Error: Failed to fetch current directory. Exiting!\n");
        exit(-1);
    }
}

void read_line(char *buffer) {
    int buffer_counter = 0;
    int input_character = getchar();
    while(input_character != EOF && input_character != '\n') {
        if (buffer_counter < MAX_INPUT_BUFFER_SIZE - 1) {
            buffer[buffer_counter] = input_character;
            buffer_counter++;
        }
        input_character = getchar();
    }
    buffer[buffer_counter] = '\0';
}

char **tokenize(char *line) {
    int position = 0;
    char **args = malloc((MAX_INPUT_BUFFER_SIZE / 2) * sizeof(char*));

    char *token = strtok_r(line, " ", &line);
    args[position++] = strdup(token);
    while ((token = strtok_r(line, " ", &line))) {
        args[position++] = strdup(token);
    }

    return args;
}

void fix_cmd_path(char *command) {
    if (command[0] != '/' && strchr(command, '/') != NULL && strstr(command, "./") != command) {
        size_t cmd_size = strlen(command);
        command = realloc(command, (cmd_size + 2) * sizeof(char));
        memmove(command + 2, command, cmd_size);
        memmove(command, "./", 2);
    }
}

char **get_shell_input() {
    char *line_input = malloc(sizeof(char) * MAX_INPUT_BUFFER_SIZE);
    if(!line_input) {
        fprintf(stderr, "Error: Cannot allocate buffer to read input. Exiting!\n");
        exit(-1);
    }

    show_prompt();
    read_line(line_input);

    if (line_input[0] == '\0') {
        return NULL;
    }

    char **args = tokenize(line_input);
    fix_cmd_path(args[0]);
    
    free(line_input);

    return args;
}
