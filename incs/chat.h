#pragma once
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "user.h"
#include "utils.h"
#include "server.h"

#define BUF_SIZE 128

typedef struct room {
    uint8_t room_number;
    bool is_secret;
    uint8_t *roomname;
    char password[16];
    Array *user_fds;
    // int user_fds[16];
} Room;

void ProcessCommand(Server *server, User *user);
void SendMsg(Server *server, User *user);