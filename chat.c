#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "chat.h"

static pthread_mutex_t lock;
static struct message *first_message;
static struct message *last_message;
static u_int message_length;

void initialize_chat(char *file_name){
    if(pthread_mutex_init(&lock, NULL) != 0){
        printf("Failed to create chat lock. Exiting...\n");
        exit(1);
    }
    if(file_name != NULL){
        // Opening an existing chat
    }else{
        // New chat
        first_message = NULL;
        last_message = NULL;
        message_length = 0;
    }
}

