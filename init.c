#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <stdarg.h>

#include "init.h"
#include "validator.h"

#define MAX_INPUT_BUFFER_SIZE 1000

void show_prompt() {
    char *buffer;
    if ((buffer = getcwd(NULL, 0)) != NULL) {
        printf("[nyush %s]$ ", basename(buffer));
    } else {
        show_invalid_msg("Error: Failed to fetch current directory. Exiting!\n");
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

void tokenize(char *line, char *args[]) {
    int position = 0;
    char *token = strtok_r(line, " ", &line);
    args[position++] = strdup(token);
    while ((token = strtok_r(line, " ", &line))) {
        args[position++] = strdup(token);
    }
    args[position] = NULL;
}

void fix_cmd_path(char *command) {
    if (command[0] != '/' && strchr(command, '/') != NULL && strstr(command, "./") != command) {
        size_t cmd_size = strlen(command);
        command = realloc(command, (cmd_size + 2) * sizeof(char));
        memmove(command + 2, command, cmd_size);
        memmove(command, "./", 2);
    }
}

int get_shell_input(char *args[]) {
    char *line_input = malloc(sizeof(char) * MAX_INPUT_BUFFER_SIZE);
    if(!line_input) {
        show_invalid_msg("Error: Cannot allocate buffer to read input. Exiting!\n");
        exit(-1);
    }

    show_prompt();
    read_line(line_input);

    if (line_input[0] == '\0') {
        return 0;
    }

    tokenize(line_input, args);
    fix_cmd_path(args[0]);
    
    free(line_input);
    return validate_args(args);
}

void clear_buffer(char *args[]) {        
    for(int i = 0; args[i] != NULL; i++) {
        args[i] = NULL;
    }
}
