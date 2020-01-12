#ifndef C_CHATTER_SERVER_H
#define C_CHATTER_SERVER_H

#define PORT "7777"
#define MAX_CONNECTION 20

// Someone sent a message
#define MESSAGE "message"
// Client telling server it is leaving
#define LEAVING "leave"
// Server telling all clients a person left
#define WHO_LEAVING "who leave"
// Server telling all client that the host left and they must go too.
#define EXIT "exit"



void startServer();

#endif
