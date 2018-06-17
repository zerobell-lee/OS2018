#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stddef.h>


typedef struct __node__ {
    int data;
    struct __node__* next;
} Node;

typedef struct __list__ {
    Node* head;
    Node* tail;
    int size;
} List;

int InitList(List* plist) {
    if (plist==NULL) return -1;
    plist->head = NULL;
    plist->tail = NULL;
    plist->size = 0;

    return 1;
}

int IsEmptyList(List* plist) {
    if (plist->size) return 0;
    else return 1;
}

int IsFullList(List* plist) {
    if ((plist->size) >= 100) return 1;
    else return 0;
}

int AddItem(List* plist, int data) {
    Node* pnode;
    if (IsEmptyList(plist)) {
        pnode = (Node *)malloc(sizeof(Node));
        pnode->data = data;
        pnode->next = NULL;

        plist->head = pnode;
        plist->tail = pnode;

        plist->size++;
        return 1;
    }
    else if (IsFullList(plist))
        return 0;
    else {
        pnode = (Node *)malloc(sizeof(Node));
        pnode->data = data;
        pnode->next = NULL;

        plist->tail->next = pnode;
        plist->tail = pnode;

        plist->size++;
        return 1;
    }
}

int RemoveItem(List* plist, int* data) {
    Node* pnode;
    if (IsEmptyList(plist)) return -1;
    else {
        pnode = plist->head;
        *data = pnode->data;
        plist->head = pnode->next;

        plist->size--;
        free(pnode);
        return 1;
    }
}

//shared variables
List* buffer;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_has_space = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_has_data = PTHREAD_COND_INITIALIZER;

void *producer(void);
void *consumer(void);

int main(void) {
    int i;
    buffer = malloc(sizeof(List));
    InitList(buffer);

    pthread_t threads[2];
    pthread_create(&threads[0], NULL, producer, NULL);
    pthread_create(&threads[1], NULL, consumer, NULL);

    for (i = 0; i<2; i++)
        pthread_join(threads[i], NULL);
    return 0;
    free(buffer);
}

void *producer(void) {
    int i;
    for (i=0; i<1000; i++) {
        pthread_mutex_lock(&mutex);
        if (IsFullList(buffer))
            pthread_cond_wait(&buffer_has_space, &mutex);
        AddItem(buffer, i);
        pthread_cond_signal(&buffer_has_data);
        pthread_mutex_unlock(&mutex);
    }
}

void *consumer(void) {
    int i, data;
    for (i=0; i<1000; i++) {
        pthread_mutex_lock(&mutex);
        if (IsEmptyList(buffer))
            pthread_cond_wait(&buffer_has_data, &mutex);
        RemoveItem(buffer, &data);
        pthread_cond_signal(&buffer_has_space);

        pthread_mutex_unlock(&mutex);
        printf("data = %d\n", data);
    }
}