#ifndef _INIT_H_
#define _INIT_H_

int get_shell_input(char line_input[]);
int get_pipe_count(char line_input[]);
void tokenize(char *line, char *delimiter, char *args[]);
void clear_buffer(char *buffer[]);
void clear_line_buffer(char *line_buffer);
void clear_args(char *args[]);

#endif