#include "../../incs/db.h"

MYSQL* DbInit() {
    MYSQL* conn, connection;

    conn = mysql_init(NULL);

    mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0);
    if (conn == NULL) {
        printf("mysql init failed\n");
        // perror(errno);

        return NULL;
    }
    mysql_query(conn, "USE chat");
    return conn;
}

bool FindUser(MYSQL* conn, char *buf) {
    char query[128];
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(query, "SELECT ID FROM user WHERE ID=\"%s\"", buf);
    if (mysql_query(conn, query)) {
        printf("query fail\n");
        return 0;
    }
    res = mysql_store_result(conn);
    if (!mysql_fetch_row(res)) {
        printf("false\n");
        return false;
    }

    return true;
}

