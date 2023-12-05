/*
 * Write a C program that will read lines from standard-in (using fgets()), that will create a linked list of the lines read. 
 * Note that you don’t know how many lines will be read from standard-in. 
 * Make sure you also deallocate the memory from the linked list.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct node_s {
    char data[1024];
    struct node_s *next;
} node_t;

void freeList(node_t *node) {
    if (node) {
        freeList(node->next);
        free(node);
    }
}

int main(void) {
    char buff[1024];
    node_t *head = NULL;
    node_t *tail = NULL;

    while (fgets(buff, sizeof(buff), stdin) != NULL) {
        size_t len = strlen(buff);
        node_t *new = NULL;

        if (len > 1 && buff[len - 1] == '\n')
            buff[len - 1] = '\0';

        new = malloc(sizeof(node_t));

        if (!new) {
            perror("Mem allocation failed.");
            exit(EXIT_FAILURE);
        }

        new->next=NULL;
        strcpy(new->data, buff);
        
        if (!head)
            head = tail = new;
        else {
            tail->next = new;
            tail = new;
        }
    }

    freeList(head);
    head = tail = NULL;

    return EXIT_SUCCESS;
}