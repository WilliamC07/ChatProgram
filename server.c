#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include "server.h"
#include "chat.h"

/**
 * Stack of currently connected clients. There are no gaps in this array.
 */
static int *client_descriptors;
/**
 * Number of client currently connected.
 */
static int number_connections;

int create_server_socket();
void handle_connection(int server_descriptor);
int accept_connection(int sd);
void handle_disconnect(int connection_index, char *message);
void send_to_clients(char *content);

/**
 * Prints out debugging messages and exit the program.
 * @param details
 */
void handle_server_failure(char *details){
    printf("Failed to start the server: %s\n", details);
    printf("Error: %d: %s\n", errno, strerror(errno));
    exit(1);
}

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
/**
 * Server main function. Will listen for incoming connections and handle communication between them
 *
 * This should be called as a separate thread.
 * @param arg Not used
 * @return Not used
 */
void *startServer(void *arg){
    int server_descriptor = create_server_socket();
    number_connections = 0;
    client_descriptors = calloc(MAX_CONNECTION, sizeof(int));

    // Thread to listen for connections
    fd_set read_fds;
    struct timespec sleep_spec;
    sleep_spec.tv_nsec = 100000000;  // .1 seconds

    while (true) {
        //select() modifies read_fds
        //we must reset it at each iteration
        FD_ZERO(&read_fds); // clears fd set
        for(int i = 0; i < number_connections; i++){
            FD_SET(client_descriptors[i], &read_fds);
        }
        FD_SET(server_descriptor, &read_fds);

        //select will block until either fd is ready
        // 60 is an arbitrary number. select() requires a number so it doesn't cause the kernal to check a lot of file descriptors
        select(60, &read_fds, NULL, NULL, NULL);

        // A client is requesting to connect to the server
        if (FD_ISSET(server_descriptor, &read_fds)) {
            handle_connection(server_descriptor);
        }

        // See which client is sending data to the server
        for(int i = 0; i < number_connections; i++){
            if(FD_ISSET(client_descriptors[i], &read_fds)){
                char *received_data = force_read_message(client_descriptors[i]);
                char message_type[MESSAGE_SIZE] = {'\0'};

                // The first line (beginning of packet to first new line character) determines what type of message
                strcpy(message_type, received_data);
                char *end_header = strchr(message_type, '\n');
                *end_header = '\0';

                // Handle each type of message
                if(strcmp(message_type, MESSAGE) == 0){
                    send_to_clients(received_data);
                }else if(strcmp(message_type, LEAVE) == 0){
                    if(i == 0){
                        // the host left

                        free(received_data);
                        break; // end server thread
                    }else{
                        // client (other than host) left
                        handle_disconnect(i, received_data);
                    }
                }else if(strcmp(message_type, JOIN) == 0){
                    send_to_clients(received_data);
                }

                free(received_data);
            }
        }

        // Put the infinite loop to sleep so it doesn't kill the CPU
        nanosleep(&sleep_spec, &sleep_spec);
    }

    // TODO: handle exiting server gracefully
}
#pragma clang diagnostic pop

void handle_disconnect(int connection_index, char *message){
    // remove descriptor
    close(client_descriptors[connection_index]);
    // shift client descriptors over to not leave gaps
    for(int i = connection_index; i < number_connections - connection_index; i++){
        client_descriptors[i] = client_descriptors[i] + 1;
    }
    number_connections--;

    // tell all the other connectors that someone left
    send_to_clients(message);
}

void send_to_clients(char *content){
    for(int i = 0; i < number_connections; i++){
        write(client_descriptors[i], content, MESSAGE_SIZE);
    }
}

void handle_connection(int server_descriptor){
    if(number_connections == 20){
        // todo: cannot accept, client must quit
        printf("Too many connections, client please go\n");
        return;
    }
    int connection = accept_connection(server_descriptor);
    client_descriptors[number_connections] = connection;
    number_connections++;
}

/**
 * Creates the server socket and allow for incoming connection.
 * Copied from DW code (he told us to)
 * @return
 */
int create_server_socket() {
    int sd, state;

    //create the socket
    sd = socket( AF_INET, SOCK_STREAM, 0 );
    if(sd < 0){
        handle_server_failure("Failed socket creation");
    }

    //setup structs for getaddrinfo
    struct addrinfo * hints, * results;
    hints = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
    hints->ai_family = AF_INET;  //IPv4 address
    hints->ai_socktype = SOCK_STREAM;  //TCP socket
    hints->ai_flags = AI_PASSIVE;  //Use all valid addresses
    getaddrinfo(NULL, PORT, hints, &results); //NULL means use local address

    //bind the socket to address and port
    state = bind(sd, results->ai_addr, results->ai_addrlen);
    if(state < 0){
        close(sd);
        handle_server_failure("Failed binding socket");
    }

    //set socket to listen state
    state = listen(sd, 10);
    if(state < 0){
        close(sd);
        handle_server_failure("Failed setting socket to listen state");
    }

    //free the structs used by getaddrinfo
    free(hints);
    freeaddrinfo(results);
    return sd;
}

/**
 * Accepts the incoming connection.
 * @param sd Server socket descriptor
 * @return Socket to communicate with the client.
 */
int accept_connection(int sd) {
    int client_socket;
    socklen_t sock_size;
    struct sockaddr_storage client_address;

    sock_size = sizeof(client_address);
    client_socket = accept(sd, (struct sockaddr *)&client_address, &sock_size);

    return client_socket;
}