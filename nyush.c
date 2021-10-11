/* References
    - https://brennan.io/2015/01/16/write-a-shell-in-c/
*/

#include <stdio.h>
#include <stdlib.h>

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
