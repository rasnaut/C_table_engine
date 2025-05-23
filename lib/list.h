#ifndef NODE_LIST_H
#define NODE_LIST_H

typedef unsigned int RelType;
typedef unsigned int InfoType;

typedef struct Node {
    RelType release;
    InfoType info;
    struct Node* next;
} Node;

Node* node_create(InfoType info, RelType release);
Node* node_insert(Node* head, InfoType info, RelType* release);
Node* node_find(Node* head, RelType release);
int node_delete(Node** head, RelType release);
void node_destroy(Node* head);

void print_list(const Node* head);


#endif
