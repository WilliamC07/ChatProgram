#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include "terminal.h"

int main() {
    enter_raw_mode();

    // Read keystroke and press "q" to quit
    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1) {
            printf("Failed to process");
            exit(1);
        }
        if(c == '\0') {
            continue;
        }else if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else if(c == 'q'){
            if (c == 'q') break;
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
    }

    return 0;
}