#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "terminal.h"

static struct termios original_terminal_attributes;

void enter_raw_mode(){
    // Keep track of original terminal settings so we can reset it when the program ends
    tcgetattr(STDIN_FILENO, &original_terminal_attributes);

    // Set to raw mode
    struct termios raw_mode_attributes = original_terminal_attributes;
    // ICANON: Read input byte rather than new line
    // ISIG: Ignore Ctrl-c and Ctrl-z signals, which sends SIGINT and SIGTSTP
    // IEXTEN: Ignore Ctrl-v
    raw_mode_attributes.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    // ICRNL: Ignore Ctrl-s and Ctrl-q, which stops/start data transmission to this program from terminal
    // Ctrl-m sends "carriage return" aka new line character. Do not want this behavior
    raw_mode_attributes.c_iflag &= ~(ICRNL | IXON);
    // Cooked move pressing enter will give \r\n to move cursor to right and down. We only want it to move down.
    raw_mode_attributes.c_oflag &= ~(OPOST);
    // minimum number of bytes of input needed before read() can return. 0 so we get every keystroke
    raw_mode_attributes.c_cc[VMIN] = 0;
    // Wait 1/10 second for user to type another character or else read() gives 0.
    raw_mode_attributes.c_cc[VTIME] = 1;

    // Apply attributes
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_mode_attributes);

    // Go back to original terminal attributes when the program exists
    atexit(disable_raw_mode);
}

void disable_raw_mode(){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_terminal_attributes);
}

void clear_terminal(){
    write(STDOUT_FILENO, "\x1b[2J", 4);
}