

#include "lib_key_array.h"

#define MAX_KEYS 3
#define MAX_CHILDREN 4

typedef struct Element {
    char* key;
    char* value;
} Element;

typedef struct Node234 {
    struct Element elements[MAX_KEYS];
    struct Node234* children[MAX_CHILDREN];
    struct Node234* parent; // Pointer to the parent node
    int key_count;
    int child_count;
} Node234;

Node234* node234_create_node(int child_count, Node234* parent);
void node234_delete_node();
void node234_add_child(Node234* parent, Node234* child);

void split_child(Node234* parent, int child_index, Node234* child);

void insert_nonfull(Node234* node, const char* key, const char* value);
void insert(Node234** root_ref, const char* key, const char* value);

void print_tree(Node234* node, int level);

void node234_destroy(Node234* root);
Element* node234_search_by_key(Node234* root, const char *key);
void node234_tree_print(Node234* node, int level);

KeyArray* node234_get_all_nodes_contained_substring(
    Node234* root, const char* substring);
KeyArray* node234_get_all_nodes_max_diff_symbols(
    const Node234* root, const char* target_key);
void node234_erase_node_by_key(Node234* root, const char* key);

void node234_print_sort_view(const Node234* root);



