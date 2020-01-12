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
static pthread_mutex_t lock;

int server_setup();
void *socket_listen(int *server_descriptor);
int server_connect(int sd);
void disconnect(int connection_index);
void send_to_clients(char *content);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
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
    pthread_create(&socket_thread, NULL, (void *(*)(void *)) socket_listen, &server_descriptor);
    fd_set read_fds;
    char received_data[MAX_LENGTH_COMMAND + 200];
    char copy[MAX_LENGTH_COMMAND + 200];
    struct timespec sleep_spec;
    sleep_spec.tv_nsec = 100000000;  // .1 seconds

    while (true) {
        pthread_mutex_lock(&lock);

        //select() modifies read_fds
        //we must reset it at each iteration
        FD_ZERO(&read_fds); // clears fd set
        for(int i = 0; i < number_connections; i++){
            FD_SET(connections[i], &read_fds);
        }

        //select will block until either fd is ready
        select(connections[number_connections - 1] + 1, &read_fds, NULL, NULL, NULL);

        // See which client is sending data to the server
        for(int i = 0; i < number_connections; i++){
            if(FD_ISSET(connections[i], &read_fds)){
                read(connections[i], received_data, sizeof(received_data));
                strcpy(copy, received_data);
                char *end_header = strchr(copy, '\n');
                *end_header = '\0';
                if(strcmp(copy, MESSAGE) == 0){
                    send_to_clients(received_data);
                }else if(strcmp(copy, LEAVING)){
                    // handle
                }
            }
        }

        pthread_mutex_unlock(&lock);
        nanosleep(&sleep_spec, &sleep_spec);
    }
}
#pragma clang diagnostic pop

void disconnect(int connection_index){

}

void send_to_clients(char *content){
    int size = strlen(content);
    for(int i = 0; i < number_connections; i++){
        write(connections[i], content, size);
    }
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