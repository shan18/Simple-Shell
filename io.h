#ifndef _IO_H_
#define _IO_H_

int replace_stdio(char *std_fn, char *stdtype);
int update_io(char *args[], char *stdio_type, int fn_idx);
int input_redirection(char *args[]);
int output_redirection(char *args[]);
void restore_stdio(int stdio_fd, char *stdtype);

#endif