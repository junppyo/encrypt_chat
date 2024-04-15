#pragma once
#include "/usr/include/mysql/mysql.h"
#include <stdio.h>
#include <errno.h>

#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASS "root"
#define DB_NAME "chat"

bool FindUser(MYSQL* conn, char *buf);
MYSQL* DbInit();