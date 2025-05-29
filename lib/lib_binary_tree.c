#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lib_binary_tree.h"

BinaryTreeNode *binary_tree_node_create(const char *key, const char *data)
{
    BinaryTreeNode *node = (BinaryTreeNode *)malloc(sizeof(BinaryTreeNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for BinaryTreeNode\n");
        return NULL;
    }
    node->key = strdup(key);
    if (!node->key) {
        fprintf(stderr, "Error: Memory allocation failed for key in BinaryTreeNode\n");
        free(node);
        return NULL;
    }
    node->data = strdup(data);
    if (!node->data) {
        fprintf(stderr, "Error: Memory allocation failed for data in BinaryTreeNode\n");
        free(node->key);
        free(node);
        return NULL;
    }
    node->left=NULL;
    node->right=NULL;
    return node;
}

int binary_tree_node_create_child_recursive(BinaryTreeNode *parent, const char *key, const char *data, size_t *height)
{
    if (!parent || !key || !data) {
        fprintf(stderr, "Error: Invalid parameters for binary_tree_node_create_child_recursive\n");
        return 0;
    }

    BinaryTreeNode **new_child = NULL;
    (*height)++;
    if(parent->key != NULL) {
        if (strcmp(key, parent->key) < 0) {
            if (parent->left) {
                return binary_tree_node_create_child_recursive(parent->left, key, data, height);
            } else {
                new_child = &(parent->left);
            }
        } else if (strcmp(key, parent->key) > 0) {
            if (parent->right) {
                return binary_tree_node_create_child_recursive(parent->right, key, data, height);
            } else {
                new_child = &(parent->right);
            }
        } else {
            // Key already exists, do not insert duplicates
            char* new_data = (char*)realloc(parent->data, strlen(data)+1);
            if (new_data) {
                parent->data = new_data;
                strcpy(parent->data, data);
            } else {
                fprintf(stderr, "Error: Memory allocation failed for data in existing BinaryTreeNode\n");
            }
            return 0;
        }
    }
    

    *new_child = binary_tree_node_create(key, data);
    if (!*new_child) {
        fprintf(stderr, "Error: Memory allocation failed for new BinaryTreeNode\n");
        return 0;
    }
    return 1; // Node created successfully
}

void binary_tree_node_destroy(BinaryTreeNode *node)
{
    if (node) {
        free(node->key);
        free(node->data);
        free(node);
    }
}

void binary_tree_node_recursive_destroy(BinaryTreeNode *node)
{
    if (node) {
        binary_tree_node_recursive_destroy(node->left);
        binary_tree_node_recursive_destroy(node->right);
        binary_tree_node_destroy(node);
    }
}

void binary_tree_node_print(const BinaryTreeNode *node, int level) {
    if (node) {
        for (int i = 0; i < level; i++) {
            printf("  ");
        }
        printf("Node: key='%s', data='%s'\n", node->key, node->data);
    }
}

void binary_tree_node_recursive_print_tree_view(const BinaryTreeNode *node, int level)
{
    if (node) {
        binary_tree_node_print(node, level);
        binary_tree_node_recursive_print_tree_view(node->left, level + 1);
        binary_tree_node_recursive_print_tree_view(node->right, level + 1);
    }
}

void binary_tree_node_recursive_print_sort_view(const BinaryTreeNode *node)
{
    if (node) {
        binary_tree_node_recursive_print_sort_view(node->left);
        binary_tree_node_print(node, 0);
        binary_tree_node_recursive_print_sort_view(node->right);
    }
}

size_t binary_tree_node_calculate_height_recursiv(const BinaryTreeNode *node) {
    if (!node) return 0;
    size_t left_height = binary_tree_node_calculate_height_recursiv(node->left);
    size_t right_height = binary_tree_node_calculate_height_recursiv(node->right);
    return 1 + (left_height > right_height ? left_height : right_height);
}

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
        tree->size += binary_tree_node_create_child_recursive(tree->root, key, data, &height);
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
    }
}

BinaryTreeNode *binary_tree_search(const BinaryTree *tree, const char *key)
{   
    if (!tree || !key) {
        fprintf(stderr, "Error: Invalid parameters for binary_tree_search\n");
        return NULL;
    }
    BinaryTreeNode *current = tree->root;
    printf("binary_tree_search 1\n");
    while (current) {
        int cmp = strcmp(key, current->key);
        printf("cmp == %d\n", cmp);
        if (cmp < 0) {
            current = current->left;
        } else if (cmp > 0) {
            current = current->right;
        } else {
            break; // Key found
        }
    }
    return current;
}

int binary_tree_get_all_nodes_contained_substring(
    const BinaryTreeNode* root, const char* substring, BinaryTree* result_tree)
{
    if (!root || !substring) {
        fprintf(stderr, "Error: Invalid parameters for binary_tree_get_all_nodes_contained_substring\n");
        return 0;
    }
    if(!root) {
        return 0; // Return empty tree if the original tree is empty
    }
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

void binary_tree_erase_node_by_key(BinaryTree *tree, const char *key)
{
    if (!tree || !key) return;
    tree->root = binary_tree_node_erase_child_by_key(tree->root, key);
    tree->size--;
    tree->height = binary_tree_calculate_height(tree);
}

BinaryTreeNode* binary_tree_node_erase_child_by_key(BinaryTreeNode *root, const char *key)
{
    if (!root) return NULL;

    int cmp = strcmp(key, root->key);
    if (cmp < 0) {
        root->left = binary_tree_node_erase_child_by_key(root->left, key);
    } else if (cmp > 0) {
        root->right = binary_tree_node_erase_child_by_key(root->right, key);
    } else {
        // Найден узел для удаления
        if (!root->left && !root->right) {
            binary_tree_node_destroy(root);
            return NULL;
        }
        else if (!root->left) {
            BinaryTreeNode* temp = root->right;
            binary_tree_node_destroy(root);
            return temp;
        }
        else if (!root->right) {
            BinaryTreeNode* temp = root->left;
            binary_tree_node_destroy(root);
            return temp;
        }
        else {
            // Найдём минимум справа
            BinaryTreeNode* min_node = root->right;
            while (min_node->left) {
                min_node = min_node->left;
            }
            // Копируем значения
            free(root->key);
            free(root->data);
            root->key = strdup(min_node->key);
            root->data = strdup(min_node->data);

            // Удалим дубликат
            root->right = binary_tree_node_erase_child_by_key(root->right, min_node->key);
        }
    }

    return root;
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


