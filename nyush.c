/* References
    - https://brennan.io/2015/01/16/write-a-shell-in-c/
*/

#include <stdio.h>
#include <stdlib.h>

#include "init.h"
#include "cmd.h"

#define EXIT_SUCCESS 0
#define EXIT_FALIURE -1

int main() {
    int status;
    char **args;

    do {
        args = get_shell_input();
        if(args != NULL) {
            status = run_cmd(args);
        }
    } while (status);

    // Free memory
    free(args);

    return EXIT_SUCCESS;
}
