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
static int message;

void initialize_display(){
    if(pthread_mutex_init(&lock, NULL) != 0){
        printf("Failed to create lock\r\n");
        exit(1);
    }
    middle.first_data = NULL;
    middle.last_data = NULL;
    message = 0;
}

/**
 * Determines the amount of lines need to print the given string to the terminal.
 * @param width Number of columns of terminal
 * @param string String to be printed
 */
int lines_needed_to_print(int width, char *string){
    int length = strlen(string);
    return (length / width) + 1;
}

void append_message(struct chat_data *new_data){
    pthread_mutex_lock(&lock);

    new_data->next = NULL;
    if(middle.first_data == NULL){
        // First message
        middle.first_data = new_data;
        middle.last_data = new_data;
    }else{
        // all other message order
        middle.last_data->next = new_data;
        middle.last_data = new_data;
    }
    message++;

    pthread_mutex_unlock(&lock);
}

void print_top_data(int width, int height){
    printf("Top bar\r\n");
}

void print_middle_data(int width, int height){
    pthread_mutex_lock(&lock);

    int total_lines_allowed = height - TOP_LINES - BOTTOM_LINES;
    if(total_lines_allowed <= 0){
        printf("Not enough space to print everything\r\n");
        exit(1);
    }

    int amount_lines_printed = 0;
    struct chat_data *current_chat_data = middle.first_data;

    int lines_needed = current_chat_data == NULL ? 0 : lines_needed_to_print(width, current_chat_data->data);
    int read = 0;

    while(current_chat_data != NULL && amount_lines_printed + lines_needed < total_lines_allowed){
        // print the username
        printf("Username: x Time: y id %d count: %d \r\n", read, message);
        amount_lines_printed++;
        // print the actual text
        printf("%s\r\n", current_chat_data->data);
        amount_lines_printed += lines_needed;

        current_chat_data = current_chat_data->next;
        read++;
    }

    // Pad rest of space to the last two lines left
    while(amount_lines_printed != total_lines_allowed){
        printf("\r\n");
        amount_lines_printed++;
    }


    pthread_mutex_unlock(&lock);
}

void print_bottom_data(int width, int height){
    printf("bottom 1/2 bar\r\n");
}

void update_screen(){
    int dimensions[2];
    get_terminal_dimensions(dimensions);
    int width = dimensions[0];
    int height = dimensions[1];

    print_top_data(width, height);
    print_middle_data(width, height);
    print_bottom_data(width, height);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void *display(void *param){
    struct timespec nano_time;
    nano_time.tv_nsec = 100000000; // .1 seconds

    while(1){
        nanosleep(&nano_time, &nano_time);
        clear_terminal();
        update_screen();
    }
}
#pragma clang diagnostic pop