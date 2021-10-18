/* References
    - https://stackoverflow.com/a/60058438/6562425
    - https://www.geeksforgeeks.org/taking-string-input-space-c-3-different-methods/
    - https://www.tutorialspoint.com/cprogramming/c_error_handling.htm
    - https://stackoverflow.com/a/2328335/6562425
    - https://brennan.io/2015/01/16/write-a-shell-in-c/
    - https://people.cs.pitt.edu/~khalifa/cs449/spr07/Assigns/Assign4/myshell.c
    - https://youtu.be/PIb2aShU_H4
    - https://www.youtube.com/watch?v=6xbLgZpOBi8
    - Students Referenced: Rakhee (N15260239)
*/

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
