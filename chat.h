#ifndef CHAT_H
#define CHAT_H
// char* length constants. These include end of string character.
#define MAX_LENGTH_USERNAME 26
#define MAX_LENGTH_MESSAGE 256
#define MAX_LENGTH_COMMAND 8

/**
 * Every message is either a:
 * - TEXT: What someone types in to send to everyone
 * - NOTIFICATION: Information on the current state of the chat (notify everyone if someone connects / leaves)
 */
enum MessageType {
    TEXT,
    NOTIFICATION
};
struct message {
    char username[MAX_LENGTH_USERNAME];  // Who sent the message. NULL if message is a NOTIFICATION type.
    enum MessageType message_type;
    char content[MAX_LENGTH_MESSAGE];  // Notification or text body
    long millisecond_time;  // Time the message was received by the server
    struct message *next;  // The following message. NULL if this message is the last one.
    struct message *previous;  // The previous message. NULL if this message is the first.
};
#endif