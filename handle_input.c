#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "handle_input.h"

const char *ESCAPED_TEXt = "ESCAPED";
char *message_to_send;
size_t length_message = -1;
// Command mode is when the user clicks ESCAPE
bool on_command_mode = false;

void handle_input(char input){
    if(input == '\0') {
        // No input given
        return;
    }else if(input == 27){
        // Clicked ESCAPE button or arrow keys
        char escape_sequence[2];
        if(read(STDIN_FILENO, escape_sequence, sizeof(char)) == -1) escape_sequence[0] = 0;
        if(read(STDIN_FILENO, escape_sequence + 1, sizeof(char)) == -1) escape_sequence[1] = 0;

        if(escape_sequence[0] == 0){
            on_command_mode = true;
            printf("Switched to command move\n");
        }else if(escape_sequence[0] == '['){
            switch(escape_sequence[1]){
                case 'A':
                    // up arrow
                    printf("up arrow\r\n");
                    break;
                case 'C':
                    // right arrow
                    printf("Right arrow\r\n");
                    break;
                case 'B':
                    // down arrow
                    printf("down arrow\r\n");
                    break;
                case 'D':
                    // left arrow
                    printf("left arrow\r\n");
                    break;
                default:
                    printf("%d - %d - %d\r\n", input, escape_sequence[0], escape_sequence[1]);
            }
        }
    }else if(iscntrl(input)){
        printf("%d\r\n", input);
        switch(input){
            case 13:
                // ENTER key pressed
                printf("Message: %s\n", message_to_send);
                free(message_to_send);
                message_to_send = NULL;
                length_message = -1;
                break;
        }
    }else{
        // ASCII [32, 126]
        if(on_command_mode){
            // User pressed ESCAPE, so the next keystroke (which is the value of "input") determines what to do
            switch(input){
                case 'i':
                    // Continue writing the message
                    break;
            }
        }else{
            // Allocate heap for string user is typing
            if(length_message == -1) {
                printf("Init\n");
                message_to_send = calloc(MAX_LENGTH_MESSAGE, sizeof(char));
                length_message = 0;
            }

            if(length_message != MAX_LENGTH_MESSAGE - 1){
                // Minus 1 since MAX_LENGTH_MESSAGE includes end of string character
                message_to_send[length_message++] = input;
                printf("message temp: %s\n", message_to_send);
            }
        }
        printf("%d ('%c')\r\n", input, input);
    }
}