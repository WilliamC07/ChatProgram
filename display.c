#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "terminal.h"
#include "display.h"

pthread_mutex_t lock;

void initialize_display_lock(){
    if(pthread_mutex_init(&lock, NULL) != 0){
        printf("Failed to create lock\r\n");
        exit(1);
    }
}

void terminal_text(enum Section section, char *change, char **new){
    pthread_mutex_lock(&lock);

    static char *top = NULL;
    static char *middle = NULL;
    static char *bottom = NULL;

    switch(section){
        case TOP:
            if(change != NULL){
                if(top != NULL) free(top);
                top = calloc(100, sizeof(char));
                strcpy(top, change);
            }
            if(new != NULL) *new = bottom;
            break;
        case MIDDLE:
            if(change != NULL){
                if(middle != NULL) free(middle);
                bottom = calloc(100, sizeof(char));
                strcpy(bottom, change);
            }
            if(new != NULL) *new = bottom;
            break;
        case BOTTOM:
            if(change != NULL){
                if(bottom != NULL) free(bottom);
                bottom = calloc(100, sizeof(char));
                strcpy(bottom, change);
            }
            if(new != NULL) *new = bottom;
            break;
    }

    pthread_mutex_unlock(&lock);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void *update_screen(){
    struct timespec nano_time;
    nano_time.tv_nsec = 100000000; // .1 seconds

    while(1){
        nanosleep(&nano_time, &nano_time);
        clear_terminal();

        char *middle_text;
        terminal_text(MIDDLE, NULL, &middle_text);
        if(middle_text == NULL){
            middle_text = "no text";
        }

        printf("%s\r\n", middle_text);
    }
}
#pragma clang diagnostic pop