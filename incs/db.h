#pragma once
#include "common.h"

#define DB_HOST "localhost"
#define DB_USER "junpyo"
#define DB_PASS "junpyo"
#define DB_NAME "chat"

unsigned char *DbGetUser(MYSQL* conn, unsigned char *buf);
int DbCreateUser(MYSQL* conn, unsigned char *user, unsigned char *pass);
MYSQL* DbInit();
int DbCreateLog(MYSQL *conn, unsigned char *name, unsigned char *pass);