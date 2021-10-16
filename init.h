#ifndef _INIT_H_
#define _INIT_H_

int get_shell_input(char *args[]);
void clear_buffer(char *args[]);
void duplicate_partial_args(char *args[], char *partial_args[], int sidx, int eidx);

#endif