#include "../../incs/utils.h"
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
#define BUF_SIZE 256

enum Flags {
    LOGOUT,
    LOGIN,
    PRIVATE,
    PUBLIC,
};

typedef struct client {
    unsigned char name[16];
    uint8_t status;
    Aes *room_aes;
} Client;

bool Run = true;
uint8_t KEY[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
unsigned char name[16];
bool private = false;
Aes *aes;

Client user;

void PrintBuf(unsigned char *buf, int len) {
    write(1, buf, len);
    write(1, "\n", 1);
}

int Receive(void *sock) {
    int n, err = 0;
    int *sock_fd = (int *)sock;
    uint8_t buf[BUF_SIZE];
    unsigned char *decrypt;
    socklen_t len;

    memset(buf, 0, BUF_SIZE);

    while (Run) {
        n = read(*sock_fd, buf, BUF_SIZE);
        if (n == 0 || n == -1) {
            Run = false;
            printf("\nServer closed\n");
            break;
        }
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
                    decrypt = Decrypt(user.room_aes, buf, n);
                    PrintBuf(decrypt, strlen(decrypt));
                    free(decrypt);
                } else {
                    PrintBuf(buf, n);
                }
            }
        } else {
            PrintBuf(buf, n);
        }
        memset(buf, 0, n);
        len = sizeof(err);
        err = getsockopt(*sock_fd, SOL_SOCKET, SO_ERROR, &err, &len);
    }
}

int SendMsg(int fd, unsigned char *buf) {
    int n, len;
    unsigned char *msg, *encrypt;

    if (!strcmp("!exit", buf)) {
        write(fd, buf, strlen(buf));
        return 0;
    } else if (!strcmp("!help", buf)) {
        printf("If you want to leave the room, type the !exit\n");
        return 0;
    }
    msg = MakeString(4, "[", user.name, "] : ", buf);
    len = strlen(msg);
    if (user.status == PRIVATE) {
        encrypt = Encrypt(user.room_aes, msg);
        
        if (len % 16 == 0) {
            n = write(fd, encrypt, len);
        }
        else {
            n = write(fd, encrypt, ((len / 16) + 1) * 16);
        }
        free(encrypt);
        encrypt = NULL;
    } else {
        n = write(fd, msg, len);
    }
    free(msg);
    msg = NULL;
}

int Send(void *sock) {
    int n;
    int *sock_fd = (int *)sock;
    unsigned char buf[BUF_SIZE];

    while (Run) {
        scanf("%s", buf);
        if (strlen(user.name) == 0) {
            strcpy(user.name, buf);
            printf("client name : %s\n", user.name);
        }
        if (user.status == PRIVATE || user.status == PUBLIC) {
            SendMsg(*sock_fd, buf);
        } else {
            n = write(*sock_fd, buf, strlen(buf));
        }
        memset(buf, 0, n);
    }
}


int main(int argc, unsigned char *argv[]) {
    int sock, inet, conn;
    struct sockaddr_in address;
    uint8_t buf[BUF_SIZE] = {0};
    pthread_t thread1, thread2;
    
    if (argc < 3) {
        printf("need address and port number\n");
        printf("./client (ADDRESS) (PORT)\n");
        return -1;
    }
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
    pthread_create(&thread1, NULL, (void*)Receive, &sock);
    pthread_create(&thread2, NULL, (void*)Send, &sock);
    pthread_join(thread1, NULL);
    // pthread_join(thread2, NULL);
    pthread_cancel(thread2);

    pthread_detach(thread1);
    pthread_detach(thread2);
    
    close(sock);
}
