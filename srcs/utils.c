#include "../incs/utils.h"

void AllocError() {
    printf("Memory allocate failed");
    exit(1);
}

Array* InitArray(size_t data_size) {
    Array *arr = (Array *)malloc(sizeof(Array));
    void **data = (void **)malloc(sizeof(void *) * 10);
    if (!arr || !data) return NULL;
    arr->capacity = 10;
    arr->size = 0;
    arr->data_size = data_size;
    arr->data = data;

    return arr;
}

void *InsertArray(Array *arr, void *data) {
    if (arr->size == arr->capacity) {
        arr->capacity *= 2;
        arr->data = (void **)realloc(arr->data, arr->capacity * sizeof(void *));
        if (arr->data == NULL) return NULL;
    }
    arr->data[arr->size] = malloc(arr->data_size);
    if (arr->data[arr->size] == NULL) return NULL;
    memcpy(arr->data[arr->size], data, arr->data_size);
    arr->size++;
    
    return arr->data[arr->size - 1];
}

void EraseArray(Array *arr, size_t n) {
    int i;
    
    free(arr->data[n]);
    arr->size--;
    for (i = n; i < arr->size; i++) {
        arr->data[i] = arr->data[i + 1];
    }
    arr->data[i] = NULL;
}

void ClearArray(Array *arr) {
    int i;

    for (i = 0; i < arr->size; i++) {
        free(arr->data[i]);
        arr->data[i] = NULL;
    }
    arr->size = 0;
    // free(arr->data);

    // free(arr);
}


char *ToHex(uint8_t *buf) {
    char *hex = malloc(sizeof(char) * (strlen(buf) * 2 + 1));
    if (!hex) return NULL;
    char *ret = hex;
    int i;

    for (i = 0; i < strlen(buf); i++) {
        sprintf(hex, "%02X", buf[i]);
        hex += 2;
    }
    *hex = '\0';
    return ret;
}

char *ToString(char *buf) {
    uint8_t *str = malloc(sizeof(char) * strlen(buf) / 2 + 1);
    if (!str) return NULL;
    int i;
    uint8_t tmp = 0;

    for (i = 0; i < strlen(buf) / 2; i++) {
        if (buf[i * 2] >= '0' && buf[i * 2] <= '9') {
            tmp = buf[i * 2] - '0';
        } else if (buf[i * 2] >= 'A' && buf[i * 2] <= 'Z') {
            tmp = buf[i * 2] - 'A' + 10;
        }
        tmp <<= 4;
        if (buf[i * 2 + 1] >= '0' && buf[i * 2 + 1] <= '9') {
            tmp += buf[i * 2 + 1] - '0';
        } else if (buf[i * 2 + 1] >= 'A' && buf[i * 2 + 1] <= 'Z') {
            tmp += buf[i * 2 + 1] - 'A' + 10;
        }
        str[i] = tmp;
    }
    str[i] = '\0';
    return str;
}

char* substr(const char *src, int m, int n)
{
    int i;
    int len = n - m;
 
    char *dest = (char*)malloc(sizeof(char) * (len + 1));
    if (!dest) return NULL;
    for (i = m; i < n && (*(src + i) != '\0'); i++)
    {
        *dest = *(src + i);
        dest++;
    }
    *dest = '\0';
 
    return dest - len;
}

char *Strcat(char *s1, char *s2) {
    char *ret = malloc(sizeof(char) * (strlen(s1) + strlen(s2) + 1));
    int i;

    for (i = 0; i < strlen(s1); i++) {
        ret[i] = s1[i];
    }

    for (i = 0; i < strlen(s2); i++) {
        ret[i + strlen(s1)] = s2[i];
    }
    ret[strlen(s1) + strlen(s2)] = '\0';

    return ret;
}