#pragma once
// #include "utils.h"
#include "common.h"
#include "time.h"

typedef struct server Server;
typedef struct array Array;

typedef struct room {
    uint16_t number;
    bool is_secret;
    char name[16];
    char password[32];
    Array *user_fds;
    int log_fd;
    // int user_fds[16];
} Room;

Room *InitRoom(Server *server, char *roomname, char *password);
Room *FindRoomByName(Array *rooms, char *name);
Room *FindRoomByNumber(Array *rooms, int n);
int JoinRoom(Server *server, User *user, char *name);
Room *MakeRoom(Server *server, char *name);
int LeaveRoom(Server *server, User *user);
int RequestRoomPass(Room *room, User *user);