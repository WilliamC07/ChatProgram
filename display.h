#include <stdbool.h>
#include "chat.h"

/**
 * Divides the terminal into 4 sections
 *
 * TOP:
 * - 1 line
 * - Right Aligned: <IP ADDRESS> | <PORT>
 * - Left Aligned: <error message if any>
 *
 * MIDDLE:
 * - Rest of lines (after compensating for TOP, MESSAGE, and COMMAND section
 *
 * BOTTOM:
 * - 2 lines
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

void append_message(struct chat_data new_data);

void *update_screen();