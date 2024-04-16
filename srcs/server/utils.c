#include "../../incs/utils.h"

void AllocError() {
    printf("Memory allocate failed");
    exit(1);
}

List* InitList(size_t size) {
    List *list = (List *)malloc(sizeof(List));

    struct node *head = (struct node *)malloc(sizeof(struct node));
    Node *tail = (Node *)malloc(sizeof(Node));

    head->prev = tail;
    head->next = tail;
    tail->prev = head;
    tail->next = head;

    list->head = head;
    list->tail = tail;

    list->size = size;

    return list;
}

void InsertList(List *list, void *data) {
    int i;
    Node *node = (Node *)malloc(sizeof(Node));
    node->data = malloc(sizeof(list->size));
    
    // memcpy((uint8_t*)node->data, (uint8_t*)data, list->size / sizeof(uint8_t));
    // node->data = node_data;

    list->tail->prev->next = node;
    node->prev = list->tail->prev;
    node->next = list->tail;
    list->tail->prev = node;
}

void EraseList(List *list, Node *node) {
    // Node *node = FindList(list, data);
    if (!node) return ;

    node->next->prev = node->prev;
    node->prev->next = node->next;
    free(node->data);
    free(node);
}

void EraseAllList(List *list) {
    if (list->head->next == list->tail) return ;

    Node* node = list->head->next;
    Node* tmp;
    while (node != list->tail) {
        tmp = node->next;
        free(node->data);
        free(node);
        node = tmp;
    }
    list->head->next = list->tail;
    list->head->prev = list->tail;
    list->tail->next = list->head;
    list->tail->prev = list->head;
}

void Pop(List *list) {
    if (list->tail->prev == list->head) return ;

    Node* node = list->tail->prev;

    node->prev->next = node->next;
    node->next->prev = node->prev;

    free(node);
}


size_t GetSizeList(List *list) {
    size_t size = 0;
    Node *node = list->head;

    while (node->next != list->tail) {
        node = node->next;
        size++;
    }
    return size;
}

Node *FindList(List *list, void *data) {
    Node *node = list->head->next;
    int i;

    while (node != list->tail) {
        for (i = 0; i < list->size; i++) {
            if (((uint8_t*)node->data)[i] != ((uint8_t*)data)[i]) {
                break;
            }
        }    
        if (i == list->size) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

void FreeList(List *list) {
    EraseAllList(list);
    free(list->head);
    free(list->tail);
    free(list);
}

Array* InitArray(size_t data_size) {
    Array *arr = (Array *)malloc(sizeof(Array));
    void **data = (void **)malloc(sizeof(void *) * 10);

    arr->capacity = 10;
    arr->size = 0;
    arr->data_size = data_size;
    arr->data = data;

    return arr;
}

void InsertArray(Array *arr, void *data) {
    if (arr->size == arr->capacity) {
        arr->capacity *= 2;
        arr->data = (void **)realloc(arr->data, arr->capacity * sizeof(void *));
        if (arr->data == NULL) AllocError();
    }
    arr->data[arr->size] = malloc(arr->data_size);
    if (arr->data[arr->size] == NULL) AllocError();
    memcpy(arr->data[arr->size], data, arr->data_size);
    arr->size++;
}

void EraseArray(Array *arr, size_t n) {
    int i;
    
    free(arr->data[n]);
    arr->size--;
    for (i = n; i < arr->size; i++) {
        arr->data[i] = arr->data[i + 1];
    }
}

void ClearArray(Array *arr) {
    int i;

    for (i = 0; i < arr->size; i++) {
        free(arr->data[i]);
    }
    arr->size = 0;
    // free(arr->data);

    // free(arr);
}


char *ToHex(uint8_t *buf) {
    char *hex = malloc(sizeof(char) * (strlen(buf) * 2 + 1));
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