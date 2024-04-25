#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

typedef struct array {
    void **data;
    size_t data_size;
    size_t size;
    size_t capacity;
} Array;

Array* InitArray(size_t data_size);
void *NewElement(Array *arr);
void *InsertArray(Array *arr, void *data);
void EraseElement(Array *arr, size_t n);
void ClearArray(Array *arr);
void FreeArray(Array *arr);

unsigned char *ToHex(uint8_t *buf);
unsigned char *ToString(unsigned char *buf);

unsigned char* substr(const unsigned char *src, int m, int n);
unsigned char* MakeString(int args, ...);