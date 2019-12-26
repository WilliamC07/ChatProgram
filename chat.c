#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "chat.h"

static pthread_mutex_t lock;
static struct message *first_message;
static struct message *last_message;
static size_t message_length;

/**
 * Initializes the chat by doing the following:
 * 1. Creates the lock to limit 1 thread access at once
 * 2. Loads in the existing chat if one was given
 * @param chat_name Name of the existing chat
 */
void initialize_chat(char *chat_name){
    if(pthread_mutex_init(&lock, NULL) != 0){
        printf("Failed to create chat lock. Exiting...\n");
        exit(1);
    }
    if(chat_name != NULL){
        // TODO: Opening an existing chat
    }else{
        // New chat
        first_message = NULL;
        last_message = NULL;
        message_length = 0;
    }
}

/**
 * Because the chat is stored as a linked list, this will add to the end of the linked list.
 * @param new_message Should pointer to struct stored on heap.
 */
void append_message(struct message *new_message){
    pthread_mutex_lock(&lock);

    new_message->next = NULL;
    if(first_message == NULL){
        // First message of the chat
        first_message = new_message;
        last_message = new_message;
    }else{
        // Not first message of chat, so it goes to end
        last_message->next = new_message;
        new_message->previous = last_message;
        last_message = new_message;
    }
    message_length++;

    pthread_mutex_unlock(&lock);
}

/**
 * Gain access to the entire chat log. Since the thread calling this now has ownership of the thread, it must also call
 * release_message_lock when done working with the chat log. Accessing the chat log should be read only.
 * @param first_message_buff
 * @param last_message_buff
 * @param message_length_buff
 */
void get_message_lock(struct message **first_message_buff, struct message **last_message_buff, size_t *message_length_buff){
    pthread_mutex_lock(&lock);
    *first_message_buff = first_message;
    *last_message_buff = last_message;
    *message_length_buff = message_length;
}

/**
 * When you are done reading the chat log from get_message_lock, you must call this to relese the lock so other threads
 * can access it.
 */
void release_message_lock(){
    pthread_mutex_unlock(&lock);
}

/**
 * Gets the number of messages sent.
 * @return number of messages sent.
 */
size_t get_message_length(){
    pthread_mutex_lock(&lock);
    size_t copy = message_length;
    pthread_mutex_unlock(&lock);
    return copy;
}

/**
 * Reads the content of a string containing the entire chat log into memory. See stringify_chat_log() to convert chat
 * to a string.
 * @param chat_log String of the chat log.
 */
void parse_chat_log(char *chat_log){

}

/**
 * Converts the entire chat log into a formatted string. To convert back to memory, use parse_chat_log(char *chat_log)
 * @return String representing the entire chat log.
 */
char *stringify_chat_log(){

}