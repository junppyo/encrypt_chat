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

char *DbGetUser(MYSQL* conn, char *buf) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *query = MakeString(3, "SELECT PW FROM user WHERE ID='", buf, "'");
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
    char *ret = malloc(sizeof(char) * strlen(row[0]) + 1);
    strcpy(ret, row[0]);
    mysql_free_result(res);
    free(query);
    return ret;
}

int DbCreateUser(MYSQL* conn, char *user, char *pass) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *query = MakeString(5, "INSERT INTO user(ID, PW) VALUES ('", user, "', '", pass, "')");

    if (mysql_query(conn, query)) {
        printf("query fail : %s\n", query);
        free(query);
        return 1;
    }
    free(query);
    return 0;
}

int DbCreateLog(MYSQL *conn, char *name, char *pass) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    if (!pass) pass = "";
    char *query = MakeString(5, "INSERT INTO log(ID, AES_KEY) VALUES ('", name, "', '", pass, "')");
    if (mysql_query(conn, query)) {
        printf("query fail : %s\n", query);
        free(query);
        return 1;
    }
    free(query);

    return 0;    
}