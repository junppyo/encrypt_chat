#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
// #include "common.h"

typedef struct node {
    void *data;
    struct node *prev;
    struct node *next;
} Node;

typedef struct list {
    size_t size;
    Node *head;
    Node *tail;
} List;

typedef struct array {
    void **data;
    size_t data_size;
    size_t size;
    size_t capacity;
} Array;

// void Clear(Array *array);
Array* InitArray(size_t data_size);
void *NewElement(Array *arr);
void *InsertArray(Array *arr, void *data);
void EraseArray(Array *arr, size_t n);
void ClearArray(Array *arr);
void FreeArray(Array *arr);


char *ToHex(uint8_t *buf);
char *ToString(char *buf);

char* substr(const char *src, int m, int n);
char* MakeString(int args, ...);