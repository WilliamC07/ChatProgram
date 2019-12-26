#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "terminal.h"
#include "display.h"
#include "chat.h"

static pthread_mutex_t lock;
// These variables should only be accessed/modified through locks.
static struct top_data top;
static struct bottom_data bottom;
static bool needs_update;
// Amount of messages to view up from last message.
static int scroll_factor;

void initialize_display(){
    if(pthread_mutex_init(&lock, NULL) != 0){
        printf("Failed to create lock\r\n");
        exit(1);
    }
    needs_update = false;
    scroll_factor = 0;
}

void view_older_messages(){
    pthread_mutex_lock(&lock);
    int message_length = get_message_length();
    if(message_length > 1){
        scroll_factor++;
    }
    pthread_mutex_unlock(&lock);
}

void view_newer_messages(){
    pthread_mutex_lock(&lock);
    if(scroll_factor != 0){
        scroll_factor--;
    }
    pthread_mutex_unlock(&lock);
}

/**
 * Call this when the user perform any action (press on key, resize window) to update the terminal display.
 * @param signal_number Ignored. For signal.h signal() purposes.
 */
void request_update(int signal_number){
    pthread_mutex_lock(&lock);
    needs_update = true;
    pthread_mutex_unlock(&lock);
}

/**
 * Determines the amount of lines need to print the given string to the terminal.
 * @param width Number of columns of terminal
 * @param string String to be printed
 */
int lines_needed_to_print(int width, struct message *to_display){
    int length = strlen(to_display->content);
    int size_message_body = (length / width) + 1;
    return size_message_body + 1;  // add one for the line showing username and date
}

void set_bottom_text(bool on_command_mode, char *text){
    pthread_mutex_lock(&lock);
    bottom.on_command_mode = on_command_mode;
    if(on_command_mode){
        strncpy(bottom.text, text, MAX_LENGTH_COMMAND);
    }else{
        strncpy(bottom.text, text, MAX_LENGTH_MESSAGE);
    }
    pthread_mutex_unlock(&lock);
}

void print_top_data(int width, int height, char *buffer){
    pthread_mutex_lock(&lock);
    // Print top bar at top left
    char *to_print = "\x1b[;1HTop bar\r\n";
    strcat(buffer, to_print);
    pthread_mutex_unlock(&lock);
}

struct message *cannot_print(struct message *last_message, int width, int *lines_available, int *lines_read_buff){
    // find the first (oldest) message that cannot fit on the screen
    struct message *oldest_msg = last_message;
    int lines_read = 0;
    while(oldest_msg != NULL && (*lines_available - lines_needed_to_print(width, oldest_msg)) >= 0){
        *lines_available -= lines_needed_to_print(width, oldest_msg);
        oldest_msg = oldest_msg->previous;
        lines_read++;
    }
    *lines_read_buff = lines_read;
    return oldest_msg;
}

void print_middle_data(int width, int height, char *buffer){
    int lines_available = height - TOP_LINES - BOTTOM_LINES;
    if(lines_available <= 0){
        char *to_print = "Not enough space to print everything\r\n";
        printf("%s\r\n", to_print);
        exit(1);
    }

    // Get messages
    struct message *first_message;
    struct message *last_message;
    size_t message_length;
    get_message_lock(&first_message, &last_message, &message_length);

    struct message *oldest_msg = last_message;
    int lines_read = 0;
    for(int i = 0; i < scroll_factor; i++){
        oldest_msg = oldest_msg->previous;
    }
    int try_available_lines = lines_available;
    struct message *new_oldest = cannot_print(oldest_msg, width, &try_available_lines, &lines_read);
    if(scroll_factor > 0){
        // Did scroll so we are covering a message, find it
        struct message *hidden = oldest_msg->next;
        if(try_available_lines >= lines_needed_to_print(width, hidden)){
            // Cannot scroll
            oldest_msg = cannot_print(oldest_msg->next, width, &lines_available, &lines_read);
            scroll_factor--;
        }else{
            lines_available = try_available_lines;
            oldest_msg = new_oldest;
        }
    }else{
        lines_available = try_available_lines;
        oldest_msg = new_oldest;
    }

    struct message *message_to_print;
    if(oldest_msg == NULL){
        // Did not find a message that cannot fit on screen, so we can print everything
        message_to_print = first_message;
    }else{
        // Can fit all message after oldest_msg
        message_to_print = oldest_msg->next;
    }
    while(message_to_print != NULL && lines_read != 0){
        // print the username
        char *heading = "Username: x Time: y\r\n";
        strcat(buffer, heading);

        // print actual message
        strcat(buffer, message_to_print->content);
        strcat(buffer, "\r\n");

        message_to_print = message_to_print->next;
        lines_read--;
    }

    // Fill rest of lines available with empty text
    while(lines_available > 0){
        strcat(buffer, "\r\n");
        lines_available--;
    }

    // Release information
    release_message_lock();
    pthread_mutex_unlock(&lock);
}

void print_bottom_data(int width, int height, char *buffer){
    pthread_mutex_lock(&lock);
    char *text = bottom.text;
    int length = strlen(text);
    // Byte layout:
    // \x1b -- escape character (1 byte)
    // [???;???H -- each question mark is one byte. 9 bytes
    // end of string character (1 byte)
    char cursor_reposition[11] = {0};

    // pad with "-"
    char *padding_help_text = bottom.on_command_mode ? "COMMAND" : "WRITE";
    int padding_amount = width - strlen(padding_help_text);
    strcat(buffer, padding_help_text);
    for(int i = 0; i < padding_amount; i++){
        strcat(buffer, "=");
    }

    if(length > width){
        // not enough space to fit everything
        text += length - width + 1; // reserve one space for cursor
        sprintf(cursor_reposition, "\x1b[%d;%dH", height, length + 1);
        strcat(buffer, text);
    }else{
        strcat(buffer, text);
        sprintf(cursor_reposition, "\x1b[%d;%dH", height, length + 1);
    }

    strcat(buffer, cursor_reposition);
    pthread_mutex_unlock(&lock);
}

void update_screen(){
    int dimensions[2];
    get_terminal_dimensions(dimensions);
    int width = dimensions[0];
    int height = dimensions[1];
    // Multiply by four since each cell of terminal can have additional information (like background color)
    char *buffer = calloc(width * height * 4, sizeof(char));

    print_top_data(width, height, buffer);
    print_middle_data(width, height, buffer);
    print_bottom_data(width, height, buffer);

    write(STDOUT_FILENO, buffer, strlen(buffer));

    free(buffer);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void *display(void *param){
    struct timespec nano_time;
    nano_time.tv_nsec = 50000000; // .01 seconds

    while(1){
        nanosleep(&nano_time, &nano_time);
        if(needs_update){
            clear_terminal();
            update_screen();
            needs_update = false;
        }
    }
}
#pragma clang diagnostic pop