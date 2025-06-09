#ifndef LIB_BINARY_TREE_H
#define LIB_BINARY_TREE_H

#include "lib_binary_tree_node.h"
#include "lib_key_array.h"

typedef struct BinaryTree {
    struct BinaryTreeNode* root;
    size_t size;
    size_t height;
} BinaryTree;

BinaryTree* binary_tree_create();

int binary_tree_insert(BinaryTree* tree, const char* key, const char* data);
void binary_tree_destroy(BinaryTree** tree);
BinaryTreeNode *binary_tree_search(const BinaryTree *tree, const char *key);

int binary_tree_get_all_nodes_contained_substring(
    const BinaryTreeNode* root, const char* substring, BinaryTree* result_tree);
KeyArray* binary_tree_get_all_nodes_max_diff_symbols(
    const BinaryTree* tree, const char* target_key);
int binary_tree_erase_node_by_key(BinaryTree* tree, const char* key);
void binary_tree_print(const BinaryTree* tree);
void binary_tree_print_sort_view(const BinaryTree* tree);
size_t binary_tree_calculate_height(const BinaryTree *node);

BinaryTree* binary_tree_create_from_file(const char* filename);

#endif // LIB_BINARY_TREE_H