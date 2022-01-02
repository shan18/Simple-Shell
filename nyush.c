#include <stdio.h>
#include <signal.h>
#include "init.h"
#include "cmd.h"

#define MAX_INPUT_BUFFER_SIZE 1000

int check_pipe(char line_input[]) {
    for (int i = 0; line_input[i] != '\0'; i++) {
        if (line_input[i] == '|')
            return 1;
    }
    return 0;
}

int main() {
    signal(SIGTSTP, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGTERM, signal_handler);

    int status = 1, shell_input_status = 0;
    char line_input[MAX_INPUT_BUFFER_SIZE];

    do {
        shell_input_status = get_shell_input(line_input);
        if(shell_input_status > 0) {
            status = check_pipe(line_input) ? run_pipe_cmd(line_input) : run_cmd(line_input);
        }
        clear_line_buffer(line_input);
        if (shell_input_status == -1)
            break;
    } while (status);
    puts("");
    return 0;
}
