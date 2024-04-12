#include "../incs/utils.h"

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