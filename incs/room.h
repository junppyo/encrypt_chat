#pragma once
// #include "utils.h"
#include "common.h"
#include "time.h"

typedef struct server Server;
typedef struct array Array;
typedef struct aes Aes;

typedef struct room {
    size_t number;
    bool is_secret;
    char name[16];
    char password[16];

    Aes *aes;
    Array *user_fds;
    int log_fd;
    // int user_fds[16];
} Room;

Room *InitRoom(Server *server, char *roomname, char *password);
void PrintRoomList(Array *rooms, User *user);
Room *FindRoomByName(Array *rooms, char *name);
Room *FindRoomByNumber(Array *rooms, int n);
int JoinRoom(Server *server, User *user, char *name);
Room *MakeRoom(Server *server, char *buf);
int LeaveRoom(Server *server, User *user);
void RequestRoomPass(Room *room, User *user);
int TryPrivateRoom(Server *server, User *user);
char *Strcat(char *s1, char *s2);