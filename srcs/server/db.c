#include "../../incs/db.h"

MYSQL* DbInit() {
    MYSQL* conn;

    conn = mysql_init(NULL);

    mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0);
    if (conn == NULL) {
        printf("mysql init failed\n");

        return NULL;
    }
    mysql_query(conn, "USE chat");
    return conn;
}

unsigned char *DbGetUser(MYSQL* conn, unsigned char *buf) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    unsigned char *query = MakeString(3, "SELECT PW FROM user WHERE ID='", buf, "'");
    if (mysql_query(conn, query)) {
        printf("query fail : %s\n", query);
        free(query);
        return NULL;
    }
    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    if (!row) {
        printf("not found\n");
        free(query);
        return NULL;
    }
    unsigned char *ret = malloc(sizeof(unsigned char) * strlen(row[0]) + 1);
    strcpy(ret, row[0]);
    mysql_free_result(res);
    free(query);
    return ret;
}

int DbCreateUser(MYSQL* conn, unsigned char *user, unsigned char *pass) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    unsigned char *query = MakeString(5, "INSERT INTO user(ID, PW) VALUES ('", user, "', '", pass, "')");

    if (mysql_query(conn, query)) {
        printf("query fail : %s\n", query);
        free(query);
        return 1;
    }
    free(query);
    return 0;
}

int DbCreateLog(MYSQL *conn, unsigned char *name, unsigned char *pass) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    if (!pass) pass = "";
    unsigned char *query = MakeString(5, "INSERT INTO log(ID, AES_KEY) VALUES ('", name, "', '", pass, "')");
    if (mysql_query(conn, query)) {
        printf("query fail : %s\n", query);
        free(query);
        return 1;
    }
    free(query);

    return 0;    
}