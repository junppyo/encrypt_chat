#pragma once
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/event.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
// #include "event.h"
#include "chat.h"
#include "utils.h"

// #define ADDRESS "192.168.1.24"
// #define ADDRESS "0.0.0.0"
#define ADDRESS "123.123.123.1"
#define PORT 8081

typedef struct server {
    int kqueue_fd;
    int sock;
    int status;
    struct kevent event_list[10];
    Array *changed;
    // List *changed;
    struct sockaddr_in server_addr;
    Array *users;
    Array *rooms;
    uint16_t room_number;
    Array *read_fds;
    // List *users;
    // List *rooms;
    // List *read_fds;
} Server;

Server *InitServer();
void AddEvents(Server *server, int sock, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
void CheckEvent(Server* server, int new_event);
int ErrorFlag(Server *server, struct kevent *event);
int ReadFlag(Server *server, struct kevent *event);
int WriteFlag(Server *server, struct kevent *event);
int ConnectClient(Server *server);