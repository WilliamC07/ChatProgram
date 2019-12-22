#ifndef CHAT_H
#define CHAT_H
#include "handle_input.h"
#define MAX_LENGTH_USERNAME 26

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