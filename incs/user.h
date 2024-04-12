#pragma once
#include "chat.h"
#include "utils.h"

enum Flags {
    KILLED,
    PASSED,
    REGISTERED
};

typedef struct user {
    int fd;
    uint8_t room_number;
    char *buf;
    bool flags;
} User;

User *NewUser(int fd);
User *UserByFd(Array *users, int fd);
// User *UserByFd(List* list, int fd);
void DeleteUserByFd(Array *users, int fd);