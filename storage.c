#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include "storage.h"
#include <stdbool.h>
#include <stdlib.h>
#include "chat.h"

static char project_directory_path[300];

/**
 * Not thread safe. Only call this once by the main thread.
 */
void initialize_storage(){
    // Get home directory (that is "~")
    strcpy(project_directory_path, getpwuid(getuid())->pw_dir);
    strcat(project_directory_path, "/.slothchat/");

    DIR *project_directory = opendir(project_directory_path);
    if(project_directory == NULL){
        // Directory does not exist, so create it
        mkdir(project_directory_path, 0700);
    }else{
        // Does exist, do nothing
        closedir(project_directory);
    }
}

void save_chat(){
    // MAX_LENGTH_CHAT_NAME already includes end of string character, so no need to add one to length
    char chatStoragePath[strlen(project_directory_path) + MAX_LENGTH_CHAT_NAME];
    strcpy(chatStoragePath, project_directory_path);
    strcat(chatStoragePath, get_chat_name());
    printf("chat: %s\n", chatStoragePath);
    int fd = open(chatStoragePath, O_TRUNC | O_CREAT | O_WRONLY, 0700);
    if(fd == -1){
        printf("Failed to create file: \"%s\". Did not save.\n", chatStoragePath);
    }
    char *stringifiedChat = stringify_chat_log();
    write(fd, stringifiedChat, strlen(stringifiedChat));
    free(stringifiedChat);
    close(fd);
}

bool does_chat_name_exist(char *chat_name){
    DIR *project_directory = opendir(project_directory_path);
    struct dirent *file = readdir(project_directory);
    bool found_file = false;

    do {
        struct stat file_metadata;
        char *file_name = file->d_name;
        char file_path[strlen(project_directory_path) + MAX_LENGTH_CHAT_NAME];
        strcpy(file_path, project_directory_path);
        strcat(file_path, file_name);
        stat(file_path, &file_metadata);

        // Make sure it is a regular file (a text file in this case)
        if(S_ISREG(file_metadata.st_mode) && strcmp(chat_name, file_name) == 0){
            // Chat log does exist
            found_file = true;
            break;
        }

        file = readdir(project_directory);
    }while(file != NULL);

    closedir(project_directory);
    return found_file;
}

void chat_file_descriptor(int *fd, off_t *size){
    char chatStoragePath[strlen(project_directory_path) + MAX_LENGTH_CHAT_NAME];
    strcpy(chatStoragePath, project_directory_path);
    strcat(chatStoragePath, get_chat_name());
    *fd = open(chatStoragePath, O_RDONLY);
    struct stat buf;
    fstat(*fd, &buf);
    *size = buf.st_size;
}