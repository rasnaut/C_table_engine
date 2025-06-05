#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lib_binary_tree.h"
#include "work_library.h"

BinaryTree *binary_tree_create()
{
    BinaryTree *tree = (BinaryTree *)malloc(sizeof(BinaryTree));
    if (!tree) {
        fprintf(stderr, "Error: Memory allocation failed for BinaryTree\n");
        return NULL;
    }
    tree->root = NULL;
    tree->size = 0;
    tree->height = 0;
    return tree;
}

int binary_tree_insert(BinaryTree *tree, const char *key, const char *data)
{
    if (!tree || !key || !data) {
        fprintf(stderr, "Error: Invalid parameters for binary_tree_insert\n");
        return -1;
    }
  
    if (!tree->root) {
        tree->root = binary_tree_node_create(key, data);
        if (!tree->root) {
            return -1; // Memory allocation failed
        }
        tree->size += 1;
    } else {
        size_t height = 1;
        tree->size += binary_tree_node_create_child_iterative(tree->root, key, data, &height);
        if (height > tree->height) {
            tree->height = height; // Update height if new node is deeper
        }
    }
    return 1; // Node inserted successfully
}

void binary_tree_destroy(BinaryTree *tree)
{
    if (tree) {
        if (tree->root) {
            binary_tree_node_recursive_destroy(tree->root);
        }
        free(tree);
        tree = NULL; // Set to NULL to avoid dangling pointer
    }
}

BinaryTreeNode *binary_tree_search(const BinaryTree *tree, const char *key)
{   
    if (!tree || !key) {
        fprintf(stderr, "Error: Invalid parameters for binary_tree_search\n");
        return NULL;
    }
    return binary_tree_node_search_node_by_key(tree->root, key);
}

int binary_tree_get_all_nodes_contained_substring(
    const BinaryTree* tree, const char* substring, BinaryTree* result_tree)
{
    if (!tree || !substring) {
        fprintf(stderr, "Error: Invalid parameters for binary_tree_get_all_nodes_contained_substring\n");
        return 0;
    }
    if(!tree->root) {
        return 0; // Return empty tree if the original tree is empty
    }
    BinaryTreeNode* root = tree->root;
    if(result_tree == NULL) {
        result_tree = binary_tree_create();
        if (!result_tree) {
            fprintf(stderr, "Error: Memory allocation failed for result BinaryTree\n");
            return 0;
        }
    }
    if(root->key && strstr(root->key, substring)) {
        binary_tree_insert(result_tree, root->key, root->data);
    }
    if(root->left ) { binary_tree_get_all_nodes_contained_substring(root->left, substring, result_tree); }
    if(root->right) { binary_tree_get_all_nodes_contained_substring(root->right, substring, result_tree); }
    
    return 1;
}


KeyArray* binary_tree_get_all_nodes_max_diff_symbols(
    const BinaryTree* tree, const char* target_key)
{
    if (!tree || !tree->root || !target_key) {
        fprintf(stderr, "Error: Invalid parameters\n");
        return 0;
    }

    const BinaryTreeNode* node = tree->root;
    const BinaryTreeNode* last_visited = NULL;
    KeyArray* result_array = key_array_create();
    if (!result_array) {
        fprintf(stderr, "Error: Memory allocation failed for result_array\n");
        return NULL;
    }

    while (node) {
        if (node->left && node->left != last_visited) {
            node = node->left;
        }
        else if (node->right && node->right != last_visited) {
            // Проверяем ключ
            int has_common = 0;
            for (const char* p = node->key; *p && !has_common; ++p) {
                if (strchr(target_key, *p))
                    has_common = 1;
            }
            if (!has_common)
                key_array_push_back(result_array, node->key);
            
            node = node->right;
        }
        else {
            // Проверяем ключ (если ещё не проверяли — например, левый самый нижний)
            if (!last_visited || (last_visited != node->right)) {
                int has_common = 0;
                for (const char* p = node->key; *p && !has_common; ++p) {
                    if (strchr(target_key, *p))
                        has_common = 1;
                }
                if (!has_common) 
                    key_array_push_back(result_array, node->key);
            }
            last_visited = node;
            node = node->parent;
        }
    }
    return result_array;
}

int binary_tree_erase_node_by_key(BinaryTree *tree, const char *key)
{
    if (!tree || !key) return 1;
    int result = binary_tree_node_erase_child_by_key_iterative(&(tree->root), key);
    if (result == 1) {
        fprintf(stderr, "Error: Key '%s' not found in the binary tree\n", key);
        return 1;
    }
    tree->size--;
    tree->height = binary_tree_calculate_height(tree);
    return 0; // Node successfully erased
}

void binary_tree_print(const BinaryTree *tree)
{
    if (!tree) {
        fprintf(stderr, "Error: Invalid BinaryTree for printing\n");
        return;
    }
    printf("Binary Tree (size=%zu, height=%zu):\n", tree->size, tree->height);
    binary_tree_node_recursive_print_tree_view(tree->root, 0);
}

void binary_tree_print_sort_view(const BinaryTree *tree)
{
    if(tree && tree->root)
        binary_tree_node_recursive_print_sort_view(tree->root);
}

size_t binary_tree_calculate_height(const BinaryTree *node)
{
    if(!node && node->root)
        return 0;

    return binary_tree_node_calculate_height_recursiv(node->root);
}

BinaryTree* binary_tree_create_from_file(const char* filename) {
    if (!filename) {
        fprintf(stderr, "Error: No filename provided\n");
        return NULL;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return NULL;
    }

    BinaryTree* tree = binary_tree_create();
    if (!tree) {
        fclose(file);
        fprintf(stderr, "Error: Could not create BinaryTree\n");
        return NULL;
    }

    char* key = NULL;
    char* data = NULL;

    while ((key = getstr(file)) != NULL) {
        data = getstr(file);
        if (!data) {
            fprintf(stderr, "Error: Missing data for key '%s'\n", key);
            free(key);
            binary_tree_destroy(tree);
            fclose(file);
            return NULL;
        }

        if (binary_tree_insert(tree, key, data) != 1) {
            fprintf(stderr, "Error: Failed to insert key '%s'\n", key);
            free(key);
            free(data);
            binary_tree_destroy(tree);
            fclose(file);
            return NULL;
        }

        free(key);
        free(data);
    }

    fclose(file);
    return tree;
}
