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

void *NewElement(Array *arr) {
    if (arr->size == arr->capacity) {
        arr->capacity *= 2;
        arr->data = (void **)realloc(arr->data, arr->capacity * sizeof(void *));
        if (arr->data == NULL) return NULL;
    }
    arr->data[arr->size] = malloc(arr->data_size);
    if (arr->data[arr->size] == NULL) return NULL;
    arr->size++;
    
    return arr->data[arr->size - 1];
}
void *InsertArray(Array *arr, void *data) {
    if (arr->size == arr->capacity) {
        arr->capacity *= 2;
        arr->data = (void **)realloc(arr->data, arr->capacity * sizeof(void *));
        if (arr->data == NULL) return NULL;
    }
    arr->data[arr->size] = data;
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
}

void FreeArray(Array *arr) {
    int i;

    for (i = 0; i < arr->size; i++) {
        free(arr->data[i]);
        arr->data[i] = NULL;
    }
    free(arr->data);
    free(arr);
}


unsigned char *ToHex(uint8_t *buf) {
    unsigned char *hex, *ret;
    int i;

    if (!buf || strlen(buf) == 0) return NULL;
    hex = malloc(sizeof(unsigned char) * (strlen(buf) * 2 + 1));
    if (!hex) return NULL;
    ret = hex;

    for (i = 0; i < strlen(buf); i++) {
        sprintf(hex, "%02X", buf[i]);
        hex += 2;
    }
    *hex = '\0';
    
    return ret;
}

unsigned char *ToString(unsigned char *buf) {
    int i;
    uint8_t *str = malloc(sizeof(unsigned char) * strlen(buf) / 2 + 1);
    uint8_t tmp = 0;

    if (!str) return NULL;

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

unsigned char* substr(const unsigned char *src, int m, int n)
{
    int i;
    int len = n - m;
 
    unsigned char *dest = (unsigned char*)malloc(sizeof(unsigned char) * (len + 1));
    if (!dest) return NULL;
    for (i = m; i < n && (*(src + i) != '\0'); i++)
    {
        *dest = *(src + i);
        dest++;
    }
    *dest = '\0';
 
    return dest - len;
}

unsigned char *MakeString(int args, ...) {
    va_list ap;
    unsigned char *ret, *tmp;
    int i, j;
    int len = 0;
    int p = 0;

    va_start(ap, args);
    for (i = 0; i < args; i++) {
        len += strlen(va_arg(ap, unsigned char*));
    }
    va_end(ap);

    ret = malloc(sizeof(unsigned char) * len + 1);
    ret[len] = '\0';

    va_start(ap, args);

    for (i = 0; i < args; i++) {
        tmp = va_arg(ap, unsigned char *);
        for (j = 0; j < strlen(tmp); j++) {
            ret[p + j] = tmp[j];
        }
        p += strlen(tmp);
    }
    va_end(ap);
    return ret;
}
