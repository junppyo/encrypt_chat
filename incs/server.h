#pragma once
#include "common.h"

#define ADDRESS "192.168.1.24"
#define PORT 8083
extern uint8_t KEY[];


typedef struct server {
    int kqueue_fd;
    int sock;
    int status;
    struct kevent event_list[10];
    Array *changed;
    struct sockaddr_in server_addr;
    Array *users;
    Array *rooms;
    uint16_t room_count;
    Array *read_fds;
    Aes *aes;
    MYSQL *db;
} Server;

Server *InitServer(int port);
void AddEvents(Server *server, int sock, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
void CheckEvent(Server* server, int new_event);
int ErrorFlag(Server *server, struct kevent *event);
int ReadFlag(Server *server, struct kevent *event);
int WriteFlag(Server *server, struct kevent *event);
int ConnectClient(Server *server);
void CloseServer(Server *server);
