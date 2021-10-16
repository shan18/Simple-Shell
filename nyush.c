/* References
    - https://stackoverflow.com/a/60058438/6562425
    - https://www.geeksforgeeks.org/taking-string-input-space-c-3-different-methods/
    - https://www.tutorialspoint.com/cprogramming/c_error_handling.htm
    - https://stackoverflow.com/a/2328335/6562425
    - https://brennan.io/2015/01/16/write-a-shell-in-c/
    - https://people.cs.pitt.edu/~khalifa/cs449/spr07/Assigns/Assign4/myshell.c
    - https://youtu.be/PIb2aShU_H4
*/

#include <stdio.h>
#include "init.h"
#include "cmd.h"

#define MAX_INPUT_BUFFER_SIZE 1000

int check_pipe(char *args[]) {
    for (int i = 0; args[i] != NULL; i++) {
        if (args[i][0] == '|')
            return 1;
    }
    return 0;
}

int main() {
    int status = 0, shell_input_status = 0;
    char *args[(MAX_INPUT_BUFFER_SIZE / 2) + 1];

    do {
        shell_input_status = get_shell_input(args);
        if(shell_input_status > 0) {
            status = check_pipe(args) ? run_pipe_cmd(args) : run_cmd(args);
        }
        clear_buffer(args);
        if (shell_input_status == -1)
            break;
    } while (status);
    return 0;
}
