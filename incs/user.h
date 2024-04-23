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
    // LOGOUT,
    TRY_PRIVATE,
    PRIVATE,
    PUBLIC,
};

typedef struct user {
    int fd;
    char name[16];
    size_t room_number;
    char *buf;
    int buf_len;
    uint8_t status;
} User;

User *NewUser(int fd);
User *UserByFd(Array *users, int fd);
void DeleteUserByFd(Array *users, int fd);
int DisconnectUser(Server *server, int fd);
void ClearUsers(Array *users);