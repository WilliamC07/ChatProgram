#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "terminal.h"
#include "display.h"

pthread_mutex_t lock;
// These variables should only be accessed/modified through locks.
static struct top_data top;
static struct middle_data middle;
static struct bottom_data bottom;

void initialize_display(){
    printf("top %p\r\n", &top);
    if(pthread_mutex_init(&lock, NULL) != 0){
        printf("Failed to create lock\r\n");
        exit(1);
    }
    middle.first_data = NULL;
    middle.last_data = NULL;
}

void append_message(struct chat_data new_data){
    pthread_mutex_lock(&lock);
    if(middle.first_data == NULL){
        // First message
        middle.first_data = &new_data;
        middle.last_data = &new_data;
    }else{
        middle.last_data->next = &new_data;
        middle.last_data = &new_data;
    }
    pthread_mutex_unlock(&lock);
}

char *get_middle_data(){
    pthread_mutex_lock(&lock);

    char *last_message = calloc(MAX_LENGTH_MESSAGE, sizeof(char));
    if(middle.first_data != NULL){
        strncpy(last_message, middle.last_data->data, MAX_LENGTH_MESSAGE);
    }

    pthread_mutex_unlock(&lock);

    return last_message;
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

        char *middle_text = get_middle_data();

        printf("%s\r\n", middle_text);
    }
}
#pragma clang diagnostic pop