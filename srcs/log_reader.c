
#include "../incs/log_reader.h"

int main(int argc, char *argv[]) {
    FILE* read_fd = fopen(argv[1], "r");
    MYSQL* conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char buf[512];

    conn = mysql_init(NULL);

    for (int i = 0 ; i < argc; i++) {
        printf("%d : %s\n", i, argv[1]);
    }
    mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0);

    mysql_query(conn, "USE chat");
    
    if (conn == NULL) {
        printf("mysql init failed\n");
        return 1;
    }

    char *query = MakeString(3, "SELECT AES_KEY from log where ID='", argv[1], "'");
    printf("query : %s\n", query);
    if (mysql_query(conn, query)) {
        printf("public room chat log\n");
        while (fgets(buf, sizeof(buf), read_fd)) {
            printf("%s\n", buf);
        }
    } else {
        printf("private room chat log\n");

        res = mysql_store_result(conn);
        row = mysql_fetch_row(res);
        Aes *aes = AesInit(ToString(row[0]));
        char *tmp;
        while (fgets(buf, sizeof(buf), read_fd)) {
            char *decrypt = Decrypt(aes, buf, strlen(buf) - 1);
            printf("%s\n", decrypt);
        }
    }

    return 0;
}
