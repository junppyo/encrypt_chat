#pragma once
#include "common.h"

#define BUF_SIZE 512

typedef struct array Array;

enum Flags {
    WAIT_ID,
    WAIT_PASS,
    WAIT_REGIST,
    LOGIN,
    PUBLIC,
    TRY_PRIVATE,
    PRIVATE
};

typedef struct user {
    int fd;
    char name[16];
    int room_number;
    char *buf;
    int buf_len;
    uint8_t status;
} User;

User *NewUser(int fd);
User *UserByFd(Array *users, int fd);
void DeleteUserByFd(Array *users, int fd);