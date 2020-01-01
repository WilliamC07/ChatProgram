#ifndef STORAGE
#define STORAGE
#include <stdbool.h>

void initialize_storage();
bool does_chat_name_exist(char *chat_name);
void save_chat();
#endif
