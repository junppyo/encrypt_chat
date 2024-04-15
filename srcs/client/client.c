#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "../../aes/aes.h"

#define ADDRESS "123.123.123.1"
#define PORT 8083
#define BUF_SIZE 512

uint8_t KEY[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };


int Receive(void *sock) {
    int n;
    int *sock_fd = (int *)sock;
    uint8_t buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);

    while (1) {
        n = read(*sock_fd, buf, BUF_SIZE);
        // n = recv(*sock_fd, buf, BUF_SIZE, 0);
        printf("%s", buf);
        // printf("received: %s\n", buf);
        memset(buf, 0, n);
    }
}

int Send(void *sock) {
    int n;
    int *sock_fd = (int *)sock;
    char buf[BUF_SIZE];
    while (1) {
        scanf("%s", buf);
        n = send(*sock_fd, buf, strlen(buf), 0);
        memset(buf, 0, n);
    }

}

int main(int argc, char *argv[]) {
    int sock, inet, conn;
    struct sockaddr_in address;
    uint8_t buf[BUF_SIZE] = {0};
    pthread_t thread1, thread2;
    Aes *aes = AesInit(KEY);
    // pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
    // if (!mutex) {
    //     printf("mutex allocate fail\n");
    //     return -1;
    // }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("socket create error\n");
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));
    address.sin_addr.s_addr = inet_addr(ADDRESS);

    conn = connect(sock, (struct sockaddr *)&address, sizeof(address));
    
    if (conn < 0) {
        printf("connect error\n");
        perror(strerror(errno));
        return -1;
    }
    // if (pthread_mutex_init(mutex, NULL) < 0) {
    //     printf("create mutex error\n");
    // }
    pthread_create(&thread1, NULL, Receive, &sock);
    pthread_create(&thread2, NULL, Send, &sock);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // while (1) {

    // }
    pthread_detach(thread1);
    pthread_detach(thread2);
    
    close(sock);
}
