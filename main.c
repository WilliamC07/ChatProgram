#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "terminal.h"
#include "display.h"
#include "handle_input.h"
#include "chat.h"

int main() {
    enter_raw_mode();

    // Display to user thread
    pthread_t display_thread;
    initialize_display();
    pthread_create(&display_thread, NULL, display, NULL);

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