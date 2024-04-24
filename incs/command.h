#pragma once
#include "common.h"

typedef struct server Server;
typedef struct user User;

void SetId(Server *server, User *user, unsigned char *buf);
int CreateUser(Server *server, User *user, unsigned char *buf);
int Lobby(Server *server, User *user, unsigned char *buf);
bool TryLogin(Server *server, User *user, unsigned char *buf);