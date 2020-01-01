#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include "terminal.h"
#include "display.h"
#include "handle_input.h"
#include "chat.h"
#include "storage.h"

int main() {
    initialize_storage();

    enter_raw_mode();

    signal(SIGWINCH, display);

    initialize_display();
    display();

    initialize_chat(NULL);

    // TODO: Listen server thread


    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1) {
            printf("Failed to process\n");
            exit(1);
        }
        handle_input(c);
    }
}