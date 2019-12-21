#include <time.h>
#include "terminal.h"



void *update_screen(){
    struct timespec nano_time;
    nano_time.tv_nsec = 100000000; // .1 seconds

    while(1){
        nanosleep(&nano_time, &nano_time);
        clear_terminal();

    }
}