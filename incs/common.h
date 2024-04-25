#pragma once

#include "/usr/include/mysql/mysql.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/event.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include "../aes/aes.h"
#include "command.h"
#include "db.h"
#include "user.h"
#include "room.h"
#include "utils.h"
#include "chat.h"
#include "server.h"

#define LOGIN_WORD_LIMIT 16
#define BUF_SIZE 512

