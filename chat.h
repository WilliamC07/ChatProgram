#ifndef CHAT_H
#define CHAT_H
// char* length constants. These include end of string character.
#define MAX_LENGTH_USERNAME 26
#define MAX_LENGTH_MESSAGE 256
#define MAX_LENGTH_COMMAND 8

enum Chat_Type{MESSAGE, NOTIFICATION};

struct chat_data {
    char user[MAX_LENGTH_USERNAME];
    enum Chat_Type chat_type;
    char data[MAX_LENGTH_MESSAGE];
    long millisecond_time;
    struct chat_data *next;
    struct chat_data *previous;
};
#endif