#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEYS 3
#define MAX_CHILDREN 4

typedef struct Node234 {
    char* keys[MAX_KEYS];
    char* values[MAX_KEYS];
    struct Node234* children[MAX_CHILDREN];
    int key_count;
    int is_leaf;
} Node234;

Node234* create_node(int is_leaf);

void split_child(Node234* parent, int i, Node234* child);

void insert_nonfull(Node234* node, const char* key, const char* value);
void insert(Node234** root_ref, const char* key, const char* value);
void print_tree(Node234* node, int level);


