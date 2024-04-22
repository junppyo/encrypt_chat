#pragma once
#include "common.h"

#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASS "root"
#define DB_NAME "chat"

char *DbGetUser(MYSQL* conn, char *buf);
int DbCreateUser(MYSQL* conn, char *user, char *pass);
MYSQL* DbInit();
int DbCreateLog(MYSQL *conn, char *name, char *pass);