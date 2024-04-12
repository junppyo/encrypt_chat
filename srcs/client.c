#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
// #include <iostream>
// #define ADDRESS "0.0.0.0"
// #define ADDRESS "123.123.123.1"
#define PORT 8081
#define BUF_SIZE 128

// using namespace std;

int main() {
    int sock, inet, conn;
    struct sockaddr_in address;
    char buf[BUF_SIZE] = {0};

    // sock = socket(AF_INET, SOCK_DGRAM, 0);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("socket create error\n");
        // return -1;
    }
// 
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr("123.123.123.1");
    // address.sin_addr.s_addr = inet_addr("127.0.0.1");
    printf("0\n");
    // std::cout << address.sin_addr << std::endl;

    conn = connect(sock, (struct sockaddr *)&address, sizeof(address));

    if (conn < 0) {
        printf("connect error\n");
        perror(strerror(errno));
        return -1;
    }
    printf("1\n");
    // while (1) {
        send(sock, "hello\n", 6, 0);
    // }
    printf("2\n");
    
    close(sock);
}