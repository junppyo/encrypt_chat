#pragma once
// #include "chat.h"
#include "utils.h"
#include <stdlib.h>
#include <stdbool.h>

#define BUF_SIZE 512

enum Flags {
    WAIT_ID,
    WAIT_PASS,
    WAIT_REGIST,
    LOGIN,
    PUBLIC,
    PRIVATE
};

typedef struct user {
    int fd;
    char name[16];
    uint8_t room_number;
    char *buf;
    uint8_t status;
} User;

User *NewUser(int fd);
User *UserByFd(Array *users, int fd);
void DeleteUserByFd(Array *users, int fd);