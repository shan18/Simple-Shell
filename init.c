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

int read_line(char buffer[]) {
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

    if (input_character == EOF)
        return 0;
    
    return 1;
}

int get_shell_input(char line_input[]) {
    show_prompt();
    int read_status = read_line(line_input);
    if (!read_status)
        return -1;
    if (line_input[0] == '\0')
        return 0;
    return 1;
}

int get_pipe_count(char line_input[]) {
    int i, count;
    for (i = 0, count = 0; line_input[i] != '\0'; line_input[i] == '|' ? count++ : 0, i++);
    return count;
}

void fix_cmd_path(char *command) {
    if (command[0] != '/' && strchr(command, '/') != NULL && strstr(command, "./") != command) {
        size_t cmd_size = strlen(command);
        command = realloc(command, (cmd_size + 2) * sizeof(char));
        memmove(command + 2, command, cmd_size);
        memmove(command, "./", 2);
    }
}

void tokenize(char *line, char *delimiter, char *args[]) {
    int position = 0;
    char *token = strtok_r(line, delimiter, &line);
    args[position++] = strdup(token);
    while ((token = strtok_r(line, delimiter, &line))) {
        args[position++] = strdup(token);
    }
    args[position] = NULL;
    fix_cmd_path(args[0]);
}

void clear_line_buffer(char *line_buffer) {
    for (int i = 0; line_buffer[i] != '\0'; i++)
        line_buffer[i] = '\0';
}

void clear_buffer(char *buffer[]) {
    for (int i = 0; buffer[i] != NULL; i++)
        buffer[i] = NULL;
}
