/* References
    - https://stackoverflow.com/a/60058438/6562425
    - https://www.geeksforgeeks.org/taking-string-input-space-c-3-different-methods/
    - https://www.tutorialspoint.com/cprogramming/c_error_handling.htm
    - https://stackoverflow.com/a/2328335/6562425
    - https://brennan.io/2015/01/16/write-a-shell-in-c/
    - https://youtu.be/PIb2aShU_H4
*/

#include <stdio.h>
#include "init.h"
#include "cmd.h"

#define MAX_INPUT_BUFFER_SIZE 1000

int main() {
    int status;
    char *args[(MAX_INPUT_BUFFER_SIZE / 2) + 1];

    do {
        if(get_shell_input(args)) {
            status = run_cmd(args);
        }
        clear_buffer(args);
    } while (status);

    return 0;
}
