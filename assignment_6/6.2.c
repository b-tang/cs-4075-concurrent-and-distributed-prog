#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* struct func: defining linked list and linked list node */
struct Node {
    int data;
    struct Node* next;
};
struct LinkedList {
    struct Node* head;
};

/* Global Variables */
struct LinkedList* L;
pthread_mutex_t lock;
int a, b;

/* method: inserting node into linked list */
void insert(int data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode -> data = data;
    newNode -> next = NULL;
    if (L -> head == NULL) {
        L -> head = newNode;
        return;
    }
    struct Node* last = L -> head;
    while (last -> next != NULL) {
        last = last -> next;
    }
    last -> next = newNode;
}   /* insert */

/* method: deletes node from linked list */
void delete(int data) {
    struct Node* temp = L->head;
    struct Node* prev;
    if (temp != NULL && temp -> data == data) {
        L -> head = temp -> next;
        free(temp);
        return;
    }
    while (temp != NULL && temp -> data != data) {
        prev = temp;
        temp = temp -> next;
    }
    if (temp == NULL) {
        return;
    }
    prev -> next = temp -> next;
    free(temp);
}   /* delete */

/* User Operation */
int member(int data) {
    struct Node* current = L -> head;
    while (current != NULL) {
        if (current -> data == data) {
            return 1;
        }
        current = current -> next;
    }
    return 0;
}   /* member */

/* method: generate random linked list */
void genRandLinkedList() {
    int i;
    for (i = 0; i < a; i++) {
        int r = rand() % 100;
        insert(r);
    }
}   /* genRandLinkedList */

void* taskQueue(void* threadid) {
    int i;
    for (i = 0; i < b; i++) {
        int r = rand() % 3;
        int data = rand() % 100;
        pthread_mutex_lock(&lock);
        switch (r) {
        case 0:
            if (member(data) == 0) {
                printf("%d is not in the list\n", data);
            }
            else {
                printf("%d is in the list\n", data);
            }
            break;
        case 1:
            insert(data);
            printf("Insert %d into the list\n", data);
            break;
        case 2:
            delete (data);
            printf("Delete %d from the list\n", data);
            break;
        }
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}   /* taskQueue */

int main(int argc, char* argv[]) {
    /* Input */
    printf("Problem 6.2");
    printf("\n-------------------------------------------------------------------\n");
    printf("Input the amount of tasks:\n");
    scanf("%d", &a);
    printf("Input the amount of threads:\n");
    scanf("%d", &b);
    printf("\nList L:");
    printf("\n");

    /* Initialization */
    L = (struct LinkedList *)malloc(sizeof(struct LinkedList));
    L -> head = NULL;

    /* Generate random linked list method called */
    genRandLinkedList();

    /* Creating & Joining threads */
    pthread_t threads[b];
    int rc;
    long t;
    for (t = 0; t < b; t++) {
        rc = pthread_create(&threads[t], NULL, taskQueue, (void *)t);
    }
    for (t = 0; t < b; t++) {
        pthread_join(threads[t], NULL);
    }

    pthread_exit(NULL);
    return 0;
}   /* main */
