#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "handle_input.h"
#include "chat.h"
#include "display.h"

void handle_escape(bool *on_command_mode){
    char escape_sequence[2];
    if(read(STDIN_FILENO, escape_sequence, sizeof(char)) == -1) escape_sequence[0] = 0;
    if(read(STDIN_FILENO, escape_sequence + 1, sizeof(char)) == -1) escape_sequence[1] = 0;

    if(escape_sequence[0] == 0){
        *on_command_mode = true;
        set_bottom_text(on_command_mode, "");
    }else if(escape_sequence[0] == '['){
        switch(escape_sequence[1]){
            case 'A':
                // up arrow
                view_older_messages();
                break;
            case 'B':
                // down arrow
                view_newer_messages();
                break;
        }
    }
}

void handle_input(char input){
    static char *message_string = NULL;
    static int message_index = -1;
    // Command mode is when the user clicks ESCAPE
    static bool on_command_mode = false;

    if(input < 32){
        // Control ASCII
        switch(input) {
            case 0:
                // No input given
                break;
            case 13: {
                // ENTER Key pressed
               if (message_string != NULL && message_string[0] != '0' && message_string[0] != ' ') {
                    // Make sure the user entered something before submitting a message
                    struct message *new_message = calloc(1, sizeof(struct message));
                    strncpy(new_message->content, message_string, MAX_LENGTH_MESSAGE);

                    append_message(new_message);

                    free(message_string);
                    message_string = NULL;
                    message_index = -1;
                }
                break;
            }
            case 27:
                // ESCAPE key: escape key pressed or escape sequence (arrow keys)
                handle_escape(&on_command_mode);
                break;
        }
    }else if(input == 127) {
        // BACKSPACE key pressed
        if (!on_command_mode && message_index > 0) {
            message_string[message_index - 1] = '\0';
            message_index--;
            set_bottom_text(on_command_mode, message_string);
        }
    }else{
        // ASCII codes [32, 126]: All printable ASCII characters
        if(on_command_mode){
            // TODO: allow scrolling
        }else{
            // Allocate heap for string user is typing
            if(message_index == -1) {
                message_string = calloc(MAX_LENGTH_MESSAGE, sizeof(char));
                message_index = 0;
            }

            if(message_index != MAX_LENGTH_MESSAGE - 1){
                // Minus 1 since MAX_LENGTH_MESSAGE includes end of string character
                message_string[message_index++] = input;
            }
            set_bottom_text(on_command_mode, message_string);
        }
    }

    request_update(0);
}