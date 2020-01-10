#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "chat.h"
#include "storage.h"

static pthread_mutex_t lock;
static struct message *first_message;
static struct message *last_message;
static char *chat_name;
static size_t message_length;
static char *username;

void parse_chat_log(char *buffer);

void initialize_mutex(){
    if(pthread_mutex_init(&lock, NULL) != 0){
        printf("Failed to create chat lock. Exiting...\n");
        exit(1);
    }
}

void initialize_new_chat(char *given_chat_name, char *given_username){
    initialize_mutex();

    chat_name = calloc(MAX_LENGTH_CHAT_NAME, sizeof(char));
    strncpy(chat_name, given_chat_name, MAX_LENGTH_CHAT_NAME);
    username = calloc(MAX_LENGTH_USERNAME, sizeof(char));
    strncpy(username, given_username, MAX_LENGTH_USERNAME);

    first_message = NULL;
    last_message = NULL;
    message_length = 0;
}

void initialize_disk_chat(char *given_chat_name){
    chat_name = calloc(MAX_LENGTH_CHAT_NAME, sizeof(char));
    strncpy(chat_name, given_chat_name, MAX_LENGTH_CHAT_NAME);
    // read the file
    int fd;
    off_t size;
    chat_file_descriptor(&fd, &size);
    char buff[size];
    read(fd, buff, size);
    buff[size - 1] = '\0';  // replace the ending end of line character with a end of string character

    // Get the username
    username = calloc(MAX_LENGTH_USERNAME, sizeof(char));
    int end_of_username_index = strchr(buff, '\n') - buff;
    strncpy(username, buff, end_of_username_index);
    parse_chat_log(buff + end_of_username_index + 1); // message content starts after end of line character from username))
}

void initialize_server_chat(char *connection_detail){

}

/**
 * Because the chat is stored as a linked list, this will add to the end of the linked list.
 * @param new_message Should pointer to struct stored on heap.
 */
void append_message(struct message *new_message){
    pthread_mutex_lock(&lock);
    strcpy(new_message->username, username);

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
 * Clears the chat locally. Used only when the user is exiting the chat (exiting the program)
 */
void clear_chat(){
    pthread_mutex_lock(&lock);
    message_length = 0;
    struct message *current = first_message;
    while(current != NULL){
        struct message *next = current->next;
        free(current);
        current = next;
    }
    free(username);
    free(chat_name);
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

char *get_chat_name(){
    return chat_name;
}

/**
 * Reads the content of a string containing the entire chat log into memory. See stringify_chat_log() to convert chat
 * to a string.
 */
void parse_chat_log(char *buffer){
    first_message = NULL;
    last_message = NULL;
    message_length = 0;
}

void parse_server_response(char *response){
    char *separator = strchr(response, '\n');
    *separator = '\0';
    char *header = response;
    char *content = separator + 1;
}

/**
 * Converts the entire chat log into a formatted string. To convert back to memory, use parse_chat_log(char *chat_log)
 * @return String representing the entire chat log.
 */
char *stringify_chat_log(){
    // Calculate size of string that will contain the chat log
    size_t size = 1; // Start at 1 to reserve space for end of string character
    // Store the current user's username
    size += MAX_LENGTH_USERNAME + 1;  // add one for new line character
    struct message *current = first_message;
    while(current != NULL){
        size += 1;  // Space for MessageType length (single character)
        size += strlen(current->username);
        size += strlen(current->content);
        size += 3;  // Space for new line character delimiting the username, message, MessageType length
        current = current->next;
    }

    char *string = calloc(size, sizeof(char));
    strcat(string, username);
    strcat(string, "\n");
    current = first_message;
    while(current != NULL){
        strcat(string, current->message_type == NOTIFICATION ? "n\n" : "t\n");
        strcat(string, current->username);
        strcat(string, "\n");
        strcat(string, current->content);
        strcat(string, "\n");
        current = current->next;
    }

    return string;
}