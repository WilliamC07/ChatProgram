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
    struct message *first_message;
    struct message *last_message;
};

struct bottom_data {
    bool on_command_mode;
    char text[MAX_LENGTH_MESSAGE];  // MAX_LENGTH_MESSAGE is greater than MAX_LENGTH_COMMAND
};

void initialize_display();

void set_bottom_text(bool on_command_mode, char *text);

void *display(void *param);