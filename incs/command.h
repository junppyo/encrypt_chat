#pragma once
#include "common.h"

typedef struct server Server;
typedef struct user User;

int SetId(Server *server, User *user, char *buf);
int CreateUser(Server *server, User *user, char *buf);
int LoginUser(Server *server, User *user, char *buf);
int Lobby(Server *server, User *user, char *buf);
bool TryLogin(Server *server, User *user, char *buf);