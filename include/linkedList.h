#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct Node {
    char* cmd;
    struct Node* next;
}Node;

void append(Node** headNode, char* cmd);
Node* lastNode(Node* headNode);

#endif
