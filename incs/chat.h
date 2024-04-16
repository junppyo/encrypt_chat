#pragma once
#include "common.h"
// #include <stdio.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <unistd.h>
// #include <string.h>
// #include <stdbool.h>
// #include <stdint.h>
// #include "user.h"
// #include "utils.h"
// #include "server.h"

#define BUF_SIZE 512
#define LOGIN_WORD_LIMIT 16

void ProcessCommand(Server *server, User *user);
void SendMsg(Server *server, User *user);