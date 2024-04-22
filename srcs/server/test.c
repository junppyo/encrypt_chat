#pragma once
// #define AES128 1
#include "../../incs/common.h"
#include <stdio.h>
// #include "../../aes/aes.h"
// uint8_t KEY[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
#include <fcntl.h>
// #include <string.h>

int *test() {
    return NULL;
}

int main() {
    MYSQL* conn;

    conn = mysql_init(NULL);

    mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0);
    if (conn == NULL) {
        printf("mysql init failed\n");

        return NULL;
    }
    mysql_query(conn, "USE chat");

    DbGetUser(conn, "qwer");
}