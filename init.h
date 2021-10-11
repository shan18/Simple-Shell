#ifndef _INIT_H_
#define _INIT_H_

void show_prompt();
void read_line(char *buffer, int max_buffer_size);
void tokenize(char *line, char **args);
void fix_cmd_path(char *command);
void get_shell_input(char **args, int max_buffer_size);
void free_args(char **args, ...);

#endif