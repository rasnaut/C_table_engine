#include "lib_tree_234.h"

Node234* create_node(int is_leaf) {
    Node234* node = (Node234*)malloc(sizeof(Node234));
    node->key_count = 0;
    node->is_leaf = is_leaf;
    for (int i = 0; i < MAX_CHILDREN; ++i) node->children[i] = NULL;
    return node;
}

void split_child(Node234* parent, int i, Node234* child) {
    Node234* new_node = create_node(child->is_leaf);
    new_node->key_count = 1;
    new_node->keys[0] = child->keys[2];
    new_node->values[0] = child->values[2];

    if (!child->is_leaf) {
        new_node->children[0] = child->children[2];
        new_node->children[1] = child->children[3];
    }

    child->key_count = 1;

    for (int j = parent->key_count; j >= i+1; j--)
        parent->children[j+1] = parent->children[j];
    parent->children[i+1] = new_node;

    for (int j = parent->key_count - 1; j >= i; j--) {
        parent->keys[j+1] = parent->keys[j];
        parent->values[j+1] = parent->values[j];
    }

    parent->keys[i] = child->keys[1];
    parent->values[i] = child->values[1];
    parent->key_count++;
}

void insert_nonfull(Node234* node, int key, void* value) {
    int i = node->key_count - 1;

    if (node->is_leaf) {
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            node->values[i + 1] = node->values[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->values[i + 1] = value;
        node->key_count++;
    } else {
        while (i >= 0 && key < node->keys[i]) i--;
        i++;
        if (node->children[i]->key_count == MAX_KEYS) {
            split_child(node, i, node->children[i]);
            if (key > node->keys[i]) i++;
        }
        insert_nonfull(node->children[i], key, value);
    }
}

void insert(Node234** root_ref, int key, void* value) {
    Node234* root = *root_ref;
    if (root->key_count == MAX_KEYS) {
        Node234* new_root = create_node(0);
        new_root->children[0] = root;
        split_child(new_root, 0, root);
        insert_nonfull(new_root, key, value);
        *root_ref = new_root;
    } else {
        insert_nonfull(root, key, value);
    }
}

// Simple traversal to visualize
void print_tree(Node234* node, int level) {
    if (!node) return;
    for (int i = 0; i < level; ++i) printf("  ");
    printf("[");
    for (int i = 0; i < node->key_count; ++i) {
        printf("%d", node->keys[i]);
        if (i != node->key_count - 1) printf(", ");
    }
    printf("]\n");
    for (int i = 0; i <= node->key_count; ++i)
        print_tree(node->children[i], level + 1);
}

void* search(Node234* node, const char* key) {
    int i = 0;
    while (i < node->key_count && key > node->keys[i]) ++i;
    if (i < node->key_count && key == node->keys[i])
        return node->data[i];
    if (node->is_leaf)
        return NULL;
    return search(node->children[i], key);
}