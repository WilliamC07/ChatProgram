#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

const char *ESCAPED_TEXt = "ESCAPED";
char *message_to_send;
// Command mode is when the user clicks ESCAPE
bool on_command_mode;

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
    }else {
        // ASCII [32, 126]
        if(on_command_mode){
            switch(input){
                case 'i':
                    break;
            }
        }else{

        }
        printf("%d ('%c')\r\n", input, input);
    }
}