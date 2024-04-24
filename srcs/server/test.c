#pragma once
// #define AES128 1
// #include "../../incs/common.h"
#include <stdio.h>
#include <unistd.h>
// #include "../../aes/aes.h"
// uint8_t KEY[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
#include <fcntl.h>

// #include <string.h>


int main(int argc, char *argv[]) {    
    int fd = open(argv[1], O_RDONLY);

    unsigned char buf[512];
    int n;
    while ((n = read(fd, buf, 512))) {
        for (int i = 0 ; i < n; i++) {
            printf("%02X ", buf[i]);
        }
        printf("\n");
    }
}