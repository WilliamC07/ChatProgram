#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "terminal.h"
#include "display.h"
#include "handle_input.h"
#include "chat.h"
#include "storage.h"
#include "server.h"

pthread_t server_thread;
bool is_host;

void print_help(){
    printf("Usage: ./output <options>\n");
    printf("./output -c <chat name> <your username> -- create a new chat and host\n");
    printf("./output -j <ipaddress> <your username> -- join a chat\n");
    printf("./output -o <existing chat name> -- opens existing chat and host\n");
    printf("./output -h -- get help for using this program");
    printf("./output -d <chat name> -- delete chat\n");
    printf("To leave the chat, press \"Control\" and \"C\"\n");
}

bool is_user_host(){
    return is_host;
}

void handleCommandArgs(int argc, char **argv){
    if(argc == 1){
        // No argument supplied, show help
        printf("No arguments given. Please run \"./output -h\" for help.\n");
        exit(0);
    }
    char *flag = argv[1];
    if(strcmp(flag, "-c") == 0){
        // Create a new chat
        // User must also provide a unique chat name and username
        // Ex: ./output -c "williams chat" william
        if(argc != 4){
            printf("Please provide the name of the chat and username. Use double quotes around the name of the chat or username if there are spaces\n");
            exit(0);
        }else{
            char *chat_name = argv[2];
            if(does_chat_name_exist(chat_name)){
                // Trying to create a chat of the same name as another. All chat name must be unique
                printf("The chat \"%s\" exists already. To view available chats, run \"ls ~/.slothchat\"\n", chat_name);
                exit(0);
            }else{
                is_host = true;
                pthread_create(&server_thread, NULL, startServer, NULL);
                // arbitrary wait time for the server to start.
                sleep(2);
                initialize_new_chat(chat_name, argv[3]);
            }
        }
    }else if(strcmp(flag, "-j") == 0){
        // Join chat on network
        // User must also provide the ip address, port number and username
        // Ex: ./output -j 127.0.0.1:5000 william
        if(argc != 4){
            printf("Please provide more details ('<ipaddress> <username>'). Failed. Exiting...\n");
            exit(0);
        }else{
            is_host = false;
            initialize_join_chat(argv[3], argv[2]);
        }
    }else if(strcmp(flag, "-o") == 0){
        // Create a new chat
        // User must also provide an additional parameter surrounded by double quotes / single quotes
        // Ex: ./output -c "williams chat"
        if(argc != 3){
            printf("Please provide the name of the chat. Use double quotes around the name of the chat if there are spaces\n");
            exit(0);
        }else{
            char *chat_name = argv[2];
            if(!does_chat_name_exist(chat_name)){
                // Trying to open chat name that doesn't exist.
                printf("The chat \"%s\" exists already. To view available chats, run \"ls ~/.slothchat\"\n", chat_name);
                exit(0);
            }else{
                is_host = true;
                pthread_create(&server_thread, NULL, startServer, NULL);
                // Arbitrary wait time for the server to finish setting up
                sleep(2);
                initialize_disk_chat(chat_name);
            }
        }
    }else if(strcmp(flag, "-h") == 0){
        // Ask for help
        print_help();
        exit(0);
    }else{
        printf("Did not understand the given arguments. Please run \"./output -h\" for help.\n");
        exit(0);
    }
}

void handle(){
    disable_raw_mode();
    printf("seg seg seg\n");
    exit(1);
}

int main(int argc, char **argv) {
    initialize_storage();
    handleCommandArgs(argc, argv);

    enter_raw_mode();

    signal(SIGWINCH, display);
    signal(SIGSEGV, handle);

    initialize_display();
    display();

    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1) {
            printf("Failed to process\n");
            exit(1);
        }
        handle_input(c);
    }
}