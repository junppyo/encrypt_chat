#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

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

List* InitList(size_t size);
Node *FindList(List *list, void *data);
void InsertList(List *list, void *data);
void EraseList(List *list, Node *node);
void EraseAllList(List *list);
size_t GetSizeList(List *list);
void Pop(List *list);
void FreeList(List *list);

void Clear(Array *array);
Array* InitArray(size_t data_size);
void InsertArray(Array *arr, void *data);
void EraseArray(Array *arr, size_t n);
void ClearArray(Array *arr);

char *ToHex(uint8_t *buf);
char *ToString(char *buf);