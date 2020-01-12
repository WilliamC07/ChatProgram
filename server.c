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
static pthread_mutex_t lock;

int server_setup();
void *socket_listen(int *server_descriptor);
int server_connect(int sd);

void startServer(){
    if(pthread_mutex_init(&lock, NULL) != 0){
        printf("Failed to create chat lock. Exiting...\n");
        exit(1);
    }

    int server_descriptor = server_setup();
    number_connections = 1; // the host is one connection already
    connections = calloc(MAX_CONNECTION, sizeof(int));

    // Thread to listen for connections
    pthread_t socket_thread;
    pthread_create(&socket_thread, NULL, socket_listen, &server_descriptor);

}

void *socket_listen(int *server_descriptor){
    while(true){
        pthread_mutex_lock(&lock);

        int connection = server_connect(*server_descriptor);
        *(connections + connection) = connection;
        connection++;

        if(number_connections == MAX_CONNECTION){
            break;
        }

        pthread_mutex_unlock(&lock);
    }
}

void error_check( int i, char *s ) {
    if ( i < 0 ) {
        printf("[%s] error %d: %s\n", s, errno, strerror(errno) );
        exit(1);
    }
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
    error_check( sd, "server socket" );
    printf("[server] socket created\n");

    //setup structs for getaddrinfo
    struct addrinfo * hints, * results;
    hints = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
    hints->ai_family = AF_INET;  //IPv4 address
    hints->ai_socktype = SOCK_STREAM;  //TCP socket
    hints->ai_flags = AI_PASSIVE;  //Use all valid addresses
    getaddrinfo(NULL, PORT, hints, &results); //NULL means use local address

    //bind the socket to address and port
    i = bind( sd, results->ai_addr, results->ai_addrlen );
    error_check( i, "server bind" );
    printf("[server] socket bound\n");

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
    error_check(client_socket, "server accept");

    return client_socket;
}

/*=========================
  client_setup
  args: int * to_server
  to_server is a string representing the server address
  create and connect a socket to a server socket that is
  in the listening state
  returns the file descriptor for the socket
  =========================*/
int client_setup(char * server) {
    int sd, i;

    //create the socket
    sd = socket( AF_INET, SOCK_STREAM, 0 );
    error_check( sd, "client socket" );

    //run getaddrinfo
    /* hints->ai_flags not needed because the client
       specifies the desired address. */
    struct addrinfo * hints, * results;
    hints = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
    hints->ai_family = AF_INET;  //IPv4
    hints->ai_socktype = SOCK_STREAM;  //TCP socket
    getaddrinfo(server, PORT, hints, &results);

    //connect to the server
    //connect will bind the socket for us
    i = connect( sd, results->ai_addr, results->ai_addrlen );
    error_check( i, "client connect" );

    free(hints);
    freeaddrinfo(results);

    return sd;
}