#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include "server.h"
#include "chat.h"

/**
 * Start the server.
 *
 * One thread will be for listening for socket connections. This will add to a list of connections. If the number of connections is > max number, then connection is refused.
 *
 * The main thread will run using SELECT. It will take what was given, and push it out to all the sockets. Server will not
 * keep track of the latest message.
 *
 *
 */

static int *connections;
static int number_connections;

int server_setup();
void handle_connection(int server_descriptor);
int server_connect(int sd);
void disconnect(int connection_index);
void send_to_clients(char *content);

char *force_read_message(int descriptor){
    int bytes_read = 0;
    char *message = calloc(MESSAGE_SIZE, sizeof(char));
    while(bytes_read != MESSAGE_SIZE){
        char temp[500] = {'\0'};
        bytes_read += read(descriptor, temp, sizeof(temp));
        strcat(message, temp);
    }
    return message;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void *startServer(void *arg){
    int server_descriptor = server_setup();
    number_connections = 0;
    connections = calloc(MAX_CONNECTION, sizeof(int));

    // Thread to listen for connections
    printf("Starting thread for %d\n", server_descriptor);
    fd_set read_fds;
    struct timespec sleep_spec;
    sleep_spec.tv_nsec = 100000000;  // .1 seconds

    while (true) {
        //select() modifies read_fds
        //we must reset it at each iteration
        FD_ZERO(&read_fds); // clears fd set
        for(int i = 0; i < number_connections; i++){
            FD_SET(connections[i], &read_fds);
        }
        FD_SET(server_descriptor, &read_fds);

        //select will block until either fd is ready
        // nfds of select is 25 for 20 connections and 1 for server descriptor. have some padding so > 21
        select(25, &read_fds, NULL, NULL, NULL);

        // there is an incoming connection
        if (FD_ISSET(server_descriptor, &read_fds)) {
            handle_connection(server_descriptor);
        }

        // See which client is sending data to the server
        for(int i = 0; i < number_connections; i++){
            if(FD_ISSET(connections[i], &read_fds)){
                char copy[MESSAGE_SIZE] = {'\0'};

                char *received_data = force_read_message(connections[i]);
                strcpy(copy, received_data);
                char *end_header = strchr(copy, '\n');
                *end_header = '\0';
                if(strcmp(copy, MESSAGE) == 0){
                    send_to_clients(received_data);
                }else if(strcmp(copy, LEAVING)){
                    // handle
                }

                free(received_data);
            }
        }

        nanosleep(&sleep_spec, &sleep_spec);
    }

    printf("end thread\n");
}
#pragma clang diagnostic pop

void disconnect(int connection_index){

}

void error_check( int i, char *s ) {
    if ( i < 0 ) {
        printf("[%s] error %d: %s\n", s, errno, strerror(errno) );
        exit(1);
    }
}

void send_to_clients(char *content){
    printf("Writing to %d connections", number_connections);
    for(int i = 0; i < number_connections; i++){
        write(connections[i], content, MESSAGE_SIZE);
    }
}

void handle_connection(int server_descriptor){
    if(number_connections == 20){
        // todo: cannot accept, client must quit
        printf("Too many connections, client please go\n");
        return;
    }
    int connection = server_connect(server_descriptor);
    printf("Accepting connection %d\n", connection);
    connections[number_connections] = connection;
    number_connections++;
}

/*=========================
  server_setup
  args:
  creates, binds a server side socket
  and sets it to the listening state
  returns the socket descriptor
  =========================*/
int server_setup() {
    int sd, i;

    //create the socket
    sd = socket( AF_INET, SOCK_STREAM, 0 );
    error_check( sd, "client socket" );

    //setup structs for getaddrinfo
    struct addrinfo * hints, * results;
    hints = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
    hints->ai_family = AF_INET;  //IPv4 address
    hints->ai_socktype = SOCK_STREAM;  //TCP socket
    hints->ai_flags = AI_PASSIVE;  //Use all valid addresses
    getaddrinfo(NULL, PORT, hints, &results); //NULL means use local address

    //bind the socket to address and port
    i = bind( sd, results->ai_addr, results->ai_addrlen );

    //set socket to listen state
    i = listen(sd, 10);
    error_check( i, "server listen" );
    printf("[server] socket in listen state\n");

    //free the structs used by getaddrinfo
    free(hints);
    freeaddrinfo(results);
    return sd;
}

/*=========================
  server_connect
  args: int sd
  sd should refer to a socket in the listening state
  run the accept call
  returns the socket descriptor for the new socket connected
  to the client.
  =========================*/
int server_connect(int sd) {
    int client_socket;
    socklen_t sock_size;
    struct sockaddr_storage client_address;

    sock_size = sizeof(client_address);
    client_socket = accept(sd, (struct sockaddr *)&client_address, &sock_size);

    return client_socket;
}