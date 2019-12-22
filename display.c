#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "terminal.h"
#include "display.h"

pthread_mutex_t lock;

void initialize_display_lock(){
    if(pthread_mutex_init(&lock, NULL) != 0){
        printf("Failed to create lock\r\n",);
        exit(1);
    }
}

void *update_screen(){
    struct timespec nano_time;
    nano_time.tv_nsec = 100000000; // .1 seconds

    while(1){
        nanosleep(&nano_time, &nano_time);
        clear_terminal();

    }
}