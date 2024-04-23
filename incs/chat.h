#pragma once
#include "common.h"

#define BUF_SIZE 512
#define LOGIN_WORD_LIMIT 16

void ProcessCommand(Server *server, User *user);
void SendMsg(Server *server, User *user);