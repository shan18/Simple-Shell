#ifndef _CMD_H_
#define _CMD_H_

void signal_handler(int sigid);
int run_cmd(char line_input[]);
int run_pipe_cmd(char line_input[]);

#endif