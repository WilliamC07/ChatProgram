#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "terminal.h"
#include "display.h"
#include "handle_input.h"

int main() {
    enter_raw_mode();
    pthread_t display_thread;
    pthread_create(&display_thread, NULL, update_screen, NULL);

    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1) {
            printf("Failed to process\n");
            exit(1);
        }
        handle_input(c);
    }
}