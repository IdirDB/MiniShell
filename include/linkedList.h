typedef struct Node {
    char* cmd;
    struct Node* next;
}Node;

void append(Node** headNode, char* cmd);
Node* lastNode(Node* headNode);
