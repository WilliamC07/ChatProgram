#include <stdbool.h>
#include "chat.h"

#define TOP_LINES 1
#define BOTTOM_LINES 2
/**
 * Divides the terminal into 4 sections
 *
 * TOP:
 * - Amount of lines: TOP_LINES
 * - Right Aligned: <IP ADDRESS> | <PORT>
 * - Left Aligned: <error message if any>
 *
 * MIDDLE:
 * - Amount of lines: Rest of lines (after compensating for TOP and BOTTOM)
 *
 * BOTTOM:
 * - Amount of lines: BOTTOM_LINES
 *
 */
enum Section {TOP, MIDDLE, BOTTOM};

struct top_data {
    char ip_address[16];  // ipv4 has 15 characters and 1 end of string character
    int port;
    char *error;  // Error message if any
};

struct middle_data {
    struct chat_data *first_data;
    struct chat_data *last_data;
};

struct bottom_data {
    bool is_command_mode;
    char *text;
};

void initialize_display();

void append_message(struct chat_data *new_data);

void *display(void *param);