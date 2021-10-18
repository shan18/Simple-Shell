#ifndef _VALIDATOR_H_
#define _VALIDATOR_H_

void show_invalid();
void show_invalid_msg(char *msg);
int validate_num_args(char *args[], int num);
int validate_args(char *args[]);
int validate_pipe_args(char *args[], int idx, int max_len);
int validate_pipe_line(char *line);
int validate_pipe_builtins(char *cmd, char *builtins[], int num_builtins);

#endif