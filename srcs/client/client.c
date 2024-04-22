#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "stdbool.h"
#include "../../aes/aes.h"

#define ADDRESS "127.0.0.1"
#define BUF_SIZE 512

enum Flags {
    LOGOUT,
    LOGIN,
    PRIVATE,
    PUBLIC,
};

typedef struct client {
    char name[16];
    uint8_t status;
    Aes *room_aes;
} Client;

uint8_t KEY[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
char name[16];
bool private = false;
Aes *aes;

Client user;

void PrintBuf(char *buf, int len) {
    write(1, buf, len);
    write(1, "\n", 1);
}

int Receive(void *sock) {
    int n;
    int *sock_fd = (int *)sock;
    uint8_t buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    int err = 0;

    while (!err) {
        n = read(*sock_fd, buf, BUF_SIZE);
        if (user.status == LOGOUT) {
            write(1, buf, n);
            if (!strncmp(buf, "Welcome", 7)) {
                user.status = LOGIN;
            }
        }
        else if (user.status == LOGIN && n == 16) {
            printf("join private\n");
            user.status = PRIVATE;
            user.room_aes = AesInit((uint8_t*)buf);
        } else if (user.status == LOGIN && !strcmp("JOIN", buf)) {
            printf("join public\n");
            user.status = PUBLIC;
        } else if (user.status == PRIVATE || user.status == PUBLIC) {
            if (!strncmp(buf, "Leave\n", 6)) {
                user.status = LOGIN;
                PrintBuf(buf, strlen(buf));
            } else {
                if (user.status == PRIVATE) {
                    char *decrypt = Decrypt(user.room_aes, buf, n);
                    PrintBuf(decrypt, strlen(decrypt));
                } else {
                    PrintBuf(buf, n);
                }
            }
        } else {
            printf("status: login\n");
            PrintBuf(buf, n);
        }
        memset(buf, 0, n);
        socklen_t len = sizeof(err);
        err = getsockopt(*sock_fd, SOL_SOCKET, SO_ERROR, &err, &len);
    }
}

int SendMsg(int fd, char *buf) {
    int n;
    if (!strcmp("exit", buf)) {
        write(fd, buf, strlen(buf));
        return 0;
    }
    int len = strlen(buf);
    if (user.status == PRIVATE) {
        uint8_t *encrypt = Encrypt(user.room_aes, buf);
        
        if (len % 16 == 0) {
            n = write(fd, encrypt, len);
        }
        else {
            n = write(fd, encrypt, ((len / 16) + 1) * 16);
        }
        free(encrypt);
        encrypt = NULL;
    } else {
        n = write(fd, buf, strlen(buf));
    }
}

int Send(void *sock) {
    int n;
    int *sock_fd = (int *)sock;
    char buf[BUF_SIZE];
    while (1) {
        scanf("%s", buf);
        if (user.status == PRIVATE || user.status == PUBLIC) {
            SendMsg(*sock_fd, buf);
        } else {
            n = write(*sock_fd, buf, strlen(buf));
        }
        memset(buf, 0, n);
    }
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("need address and port number\n");
        printf("./client (ADDRESS) (PORT)\n");
        return -1;
    }
    int sock, inet, conn;
    struct sockaddr_in address;
    uint8_t buf[BUF_SIZE] = {0};
    pthread_t thread1, thread2;
    user.status = LOGOUT;
    aes = AesInit(KEY);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("socket create error\n");
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[2]));
    address.sin_addr.s_addr = inet_addr(argv[1]);

    conn = connect(sock, (struct sockaddr *)&address, sizeof(address));
    
    if (conn < 0) {
        printf("connect error\n");
        perror(strerror(errno));
        return -1;
    }
    // if (pthread_mutex_init(mutex, NULL) < 0) {
    //     printf("create mutex error\n");
    // }
    // pthread_create(&thread1, NULL, Receive, &sock);
    // pthread_create(&thread2, NULL, Send, &sock);
    pthread_create(&thread1, NULL, (void*)Receive, &sock);
    pthread_create(&thread2, NULL, (void*)Send, &sock);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // while (1) {

    // }
    pthread_detach(thread1);
    pthread_detach(thread2);
    
    close(sock);
}
