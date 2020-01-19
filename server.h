#ifndef C_CHATTER_SERVER_H
#define C_CHATTER_SERVER_H

#define PORT "7777"
#define MAX_CONNECTION 20

#define MESSAGE_SIZE 300  // amount of bytes that will be sent over. pads with '\0' (end of string character)

// Someone sent a message
#define MESSAGE "message"
// Someone joined the chat
#define JOIN "join"
// Telling server that the client is leaving or the server telling client to leave.
#define LEAVE "leave"
// Server telling client of the status (Ex. someone left)
#define SERVER_STATUS "status"

char *force_read_message(int descriptor);
void *startServer(void *arg);

#endif
