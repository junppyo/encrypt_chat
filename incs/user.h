#pragma once
#include "common.h"

#define BUF_SIZE 512

typedef struct server Server;
typedef struct array Array;

enum Flags {
    WAIT_ID,
    WAIT_PASS,
    WAIT_REGIST,
    LOGIN,
    TRY_PRIVATE,
    PRIVATE,
    PUBLIC,
};

typedef struct user {
    int fd;
    unsigned char name[16];
    size_t room_number;
    unsigned char *buf;
    unsigned char buf_len;
    uint8_t status;
} User;

User *NewUser(int fd);
User *UserByFd(Array *users, int fd);
void DeleteUserByFd(Array *users, int fd);
int DisconnectUser(Server *server, int fd);
void FreeUsers(Array *users);