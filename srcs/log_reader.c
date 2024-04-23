
#include "../incs/log_reader.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("./reader (filename)\n");
        printf("Please input file name\n");
        return 1;
    }
    FILE* read_fd = fopen(argv[1], "r");
    if (!read_fd) {
        printf("Don't exist filename\n");
        return 1;
    }
    MYSQL* conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char buf[512];

    conn = mysql_init(NULL);

    mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0);

    mysql_query(conn, "USE chat");
    
    if (conn == NULL) {
        printf("mysql init failed\n");
        return 1;
    }

    char *query = MakeString(3, "SELECT AES_KEY from log where ID='", argv[1], "'");
    printf("query : %s\n", query);
    if (mysql_query(conn, query)) {
        printf("query failed : %s\n", query);
        free(query);
        return 1;
    } else {

        res = mysql_store_result(conn);
        row = mysql_fetch_row(res);
        if (!row) {
            printf("public room chat log\n");
            while (fgets(buf, sizeof(buf), read_fd)) {
                printf("%s\n", buf);
            }
        } else {
            Aes *aes = AesInit(ToString(row[0]));
            printf("private room chat log\n");
            char *tmp;
            while (fgets(buf, sizeof(buf), read_fd)) {
                char *decrypt = Decrypt(aes, buf, strlen(buf) - 1);
                printf("%s\n", decrypt);
            }
        }
        free(query);
    }

    return 0;
}
