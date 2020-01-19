#ifndef C_CHATTER_SERVER_H
#define C_CHATTER_SERVER_H

#define PORT "7777"
#define MAX_CONNECTION 20

#define MESSAGE_SIZE 300  // amount of bytes that will be sent over. pads with '\0' (end of string character)

// Someone sent a message
#define MESSAGE "message"
// Client telling server it is leaving
#define LEAVING "leave"
// Server telling all clients a person left
#define WHO_LEAVING "who leave"
// Server telling all client that the host left and they must go too.
#define EXIT "exit"

char *force_read_message(int descriptor);
void error_check( int i, char *s );
void *startServer(void *arg);

#endif
