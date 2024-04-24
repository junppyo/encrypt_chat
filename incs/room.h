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
    unsigned char name[16];
    unsigned char password[16];

    Aes *aes;
    Array *user_fds;
    int log_fd;
} Room;

Room *InitRoom(Server *server, unsigned char *roomname, unsigned char *password);
void PrintRoomList(Array *rooms, User *user);
Room *FindRoomByName(Array *rooms, unsigned char *name);
Room *FindRoomByNumber(Array *rooms, int n);
int JoinRoom(Server *server, User *user, unsigned char *name);
Room *MakeRoom(Server *server, unsigned char *buf);
int LeaveRoom(Server *server, User *user);
void RequestRoomPass(Room *room, User *user);
int TryPrivateRoom(Server *server, User *user);
void FreeRooms(Array *rooms);
unsigned char *Strcat(unsigned char *s1, unsigned char *s2);