#include "../../incs/db.h"

MYSQL* DbInit() {
    MYSQL* conn, connection;

    conn = mysql_init(NULL);

    mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0);
    if (conn == NULL) {
        printf("mysql init failed\n");

        return NULL;
    }
    mysql_query(conn, "USE chat");
    return conn;
}

char *DbGetUser(MYSQL* conn, char *buf) {
    char query[128];
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(query, "SELECT PW FROM user WHERE ID=\"%s\"", buf);
    if (mysql_query(conn, query)) {
        printf("query : %s\n", query);
        printf("query fail\n");
        return NULL;
    }
    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    if (!row) {
        printf("not found\n");
        return NULL;
    }
    char *ret = malloc(sizeof(char) * strlen(row[0]));
    printf("row[0] len: %d\n", strlen(row[0]));
    memcpy(ret, row[0], sizeof(char) * strlen(row[0]));
    return ret;
}

int DbCreateUser(MYSQL* conn, char *user, char *pass) {
    char query[128];
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(query, "INSERT INTO user(ID, PW) VALUES (\"%s\", \"%s\");", user, pass);
    printf("query : %s\n", query);
    if (mysql_query(conn, query)) {
        printf("query fail\n");
        return 1;
    }
    return 0;
}
