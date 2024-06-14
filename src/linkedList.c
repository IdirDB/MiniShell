#include <stdio.h>
#include <stdlib.h>
#include "linkedList.h"

void append(Node** headNode, char* cmd) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("malloc failed 1");
        exit(EXIT_FAILURE);
    }
    newNode->cmd = cmd;
    newNode->next = NULL;

    if (*headNode == NULL) {
        *headNode = newNode;
        return;
    }
    
    Node* last = *headNode; 
    while (last != NULL && last->next != NULL) {
        last = last->next;
    }
    last->next = newNode;
}
