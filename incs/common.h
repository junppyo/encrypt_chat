#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/event.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include "command.h"
#include "user.h"
#include "room.h"
#include "utils.h"
#include "db.h"
#include "../aes/aes.h"
#include "server.h"
#include <signal.h>

#define LOGIN_WORD_LIMIT 16
#define BUF_SIZE 512

int SetId(Server *server, User *user, char *buf);
int CreateUser(Server *server, User *user, char *buf);
bool TryLogin(Server *server, User *user, char *buf);