#pragma once
#include "common.h"

typedef struct server Server;
typedef struct array Array;
typedef struct aes Aes;
typedef struct user User;

typedef struct room {
    size_t number;
    bool is_secret;
    unsigned char name[16];
    unsigned char password[16];

    Aes *aes;
    Array *user_fds;
    int log_fd;
} Room;

Room *InitRoom(Server *server, unsigned char *roomname, unsigned char *password);
Room *FindRoomByName(Array *rooms, unsigned char *name);
Room *FindRoomByNumber(Array *rooms, int n);
Room *MakeRoom(Server *server, unsigned char *buf);
int JoinRoom(Server *server, User *user, unsigned char *name);
int LeaveRoom(Server *server, User *user);
void RequestRoomPass(Room *room, User *user);
int TryPrivateRoom(Server *server, User *user);
void FreeRooms(Array *rooms);
void PrintRoomList(Array *rooms, User *user);