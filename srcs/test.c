// #define AES128 1
#include "../incs/utils.h"
#include <stdio.h>
#include "../aes/aes.h"

int main() {
    char *buf = "1234789456789123qwertuopasdfhjlxcvnm,zxqiorusadfhjkdls";
    Aes *aes = AesInit();
    // printf("init\n");
    printf("%s\n", buf);
    char *cipher = Encrypt(aes, buf);
    printf("%s\n", cipher);
    char *plain = Decrypt(aes, cipher);
    printf("%s\n", plain);
    return 0;
}