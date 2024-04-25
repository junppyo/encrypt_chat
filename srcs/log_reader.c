#include "../incs/log_reader.h"

int main(int argc, unsigned char *argv[]) {
    MYSQL* conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    unsigned char len, buf[512], *query, *str, *decrypt;
    Aes *aes;
    int fd = open(argv[1], O_RDONLY);

    if (argc < 2) {
        printf("./reader (filename)\n");
        printf("Please input file name\n");
        return 1;
    }
    if (!fd) {
        printf("Don't exist filename\n");
        return 1;
    }

    conn = mysql_init(NULL);
    mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0);
    mysql_query(conn, "USE chat");

    if (conn == NULL) {
        printf("mysql init failed\n");
        return 1;
    }

    query = MakeString(3, "SELECT AES_KEY from log where NAME='", argv[1], "'");
    printf("query : %s\n", query);
    if (mysql_query(conn, query)) {
        printf("query failed : %s\n", query);
        free(query);
        return 1;
    }
    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    if (!row) {
        printf("public room chat log\n");
        read(fd, &len, 1);
        while (read(fd, buf, len)) {
            printf("%s\n", buf);
            memset(buf, 0, len);
            read(fd, &len, 1);
            if (len == 0) {
                if (!read(fd, &len, 1))
                    break;
            }
        }
    } else {
        str = ToString(row[0]);
        aes = AesInit(str);
        free(str);
        printf("private room chat log\n");
        
        read(fd, &len, 1);
        while (read(fd, buf, len)) {
            decrypt = Decrypt(aes, buf, len);
            printf("%s\n", decrypt);
            memset(buf, 0, len);
            free(decrypt);
            read(fd, &len, 1);
            if (len == 0) {
                if (!read(fd, &len, 1))
                    break;
            }
        }
        free(aes);
    }
    free(query);
    mysql_free_result(res);
    mysql_close(conn);

    return 0;
}

