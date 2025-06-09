#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lib_binary_tree_node.h"

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
    node->parent=NULL;
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

int binary_tree_node_create_child_iterative(BinaryTreeNode *parent, const char *key, const char *data, size_t *height) {
    if (!parent || !key || !data) {
        fprintf(stderr, "Error: Invalid parameters for binary_tree_node_create_child_iterative\n");
        return 0;
    }

    BinaryTreeNode *current = parent;
    BinaryTreeNode **leaf = NULL;
    while (current) {
        (*height)++;

        int cmp = strcmp(key, current->key);
             if (cmp < 0) { leaf = &current->left ; } 
        else if (cmp > 0) { leaf = &current->right; } 
        else {
            // Ключ уже существует → обновляем данные
            char* new_data = realloc(current->data, strlen(data) + 1);
            if (new_data) {
                current->data = new_data;
                strcpy(current->data, data);
            } else {
                fprintf(stderr, "Error: Memory allocation failed for data in existing BinaryTreeNode\n");
            }
            return 0; // Обновлено, но не создано нового узла
        }

        if (*leaf) {
                current = *leaf;
        } else {
            *leaf = binary_tree_node_create_child(current, key, data); // Создаем нового потомка
            if (!*leaf)
                return 0;
            
            return 1; // Успешно добавлен
        }
    }
    return 0; // Теоретически не должно дойти сюда
}

BinaryTreeNode * binary_tree_node_create_child(BinaryTreeNode *parent, const char *key, const char *data)
{
    BinaryTreeNode *leaf = binary_tree_node_create(key, data);
    if (!leaf) {
        fprintf(stderr, "Error: Memory allocation failed for new BinaryTreeNode\n");
        return NULL;
    }
    leaf->parent = parent;
    return leaf; // Успешно добавлен
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

void binary_tree_node_iterative_destroy(BinaryTreeNode* root) {
    if (!root) return;

    BinaryTreeNode* node = root;
    while (node) {
             if (node->left ) { node = node->left ; }
        else if (node->right) { node = node->right; }
        // У узла нет детей — удаляем его и возвращаемся к родителю
        else {
            BinaryTreeNode* parent = node->parent;
            if (parent) {
                     if (parent->left  == node) parent->left  = NULL;
                else if (parent->right == node) parent->right = NULL;
            }

            binary_tree_node_destroy(node);
            node = parent;
        }
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

void binary_tree_node_iterative_print_tree_view(const BinaryTreeNode* root) {
    if (!root) return;
    int level = 0;
    BinaryTreeNode* node = (BinaryTreeNode*)root;
    BinaryTreeNode* tmpNodeLeft = NULL;
    BinaryTreeNode* tmpNodeRight = NULL;
    char printed = 0;
    while (node) {
        if(!printed)
            binary_tree_node_print(node, level);
        if (node->left && node->left != tmpNodeLeft) { 
            node = node->left;
            level++;
            printed = 0; 
        }
        else if (node->right && node->right != tmpNodeRight)  { 
            node = node->right; 
            level++; 
            printed = 0;
        }
        else if(node->parent) {
            if (node == node->parent->right) 
                tmpNodeRight = node;
            
            node = node->parent;
            tmpNodeLeft = node->left;
            level--;
            printed = 1;
        } else {
            node = node->parent;
        }
    }
}

void binary_tree_node_iterative_print_sort_view(const BinaryTreeNode *root)
{
    if (!root) return;
    int level = 0;
    BinaryTreeNode* node = (BinaryTreeNode*)root;
    BinaryTreeNode* tmpNodeLeft = NULL;
    BinaryTreeNode* tmpNodeRight = NULL;
    char printed = 0;
    while (node) {
             if (node->left  && node->left  != tmpNodeLeft ) { node = node->left; }
        else if (node->right && node->right != tmpNodeRight) { node = node->right; }
        else if(node->parent) {
            if(!printed)
                binary_tree_node_print(node, 0);
            if (node == node->parent->right) {
                tmpNodeRight = node;
                printed = 1;
            } else if (node == node->parent->left) {
                printed = 0;
            }
            
            node = node->parent;
            tmpNodeLeft = node->left;
            level--;
        } else {
            node = node->parent;
        }
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

BinaryTreeNode *binary_tree_node_search_node_by_key(const BinaryTreeNode *start_node, const char *key)
{
    BinaryTreeNode *current = (BinaryTreeNode*)start_node;
    while (current)
    {
        int cmp = strcmp(key, current->key);
             if (cmp < 0) { current = current->left; }
        else if (cmp > 0) { current = current->right;}
        else
            break; // Key found
    }
    return current;
}

BinaryTreeNode *binary_tree_node_search_node_by_with_substring(const BinaryTreeNode *root, const char *substring)
{
    BinaryTreeNode *current = (BinaryTreeNode*)root;
    while (current)
    {
        char* cmp = strstr(substring, current->key);
        if(cmp == NULL) {
            current = current->left; // Move to left subtree
        } else {
            break;
        }
        
        // else if (cmp > 0) {
        //     current = current->right; // Move to right subtree
        //          if (cmp < 0) { current = current->left; }
        //     else if (cmp > 0) { current = current->right;}
        //     else
        //         break; // Key found
        // }
    }
    return current;
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

int binary_tree_node_erase_child_by_key_iterative(BinaryTreeNode** root, const char* key) {
    
    BinaryTreeNode* current = binary_tree_node_search_node_by_key(*root, key);
    
    // Ключ не найден
    if (!current)
        return 1;

    BinaryTreeNode* parent = current->parent;
    
    if (!current->left || !current->right) {
        BinaryTreeNode* child = current->left ? current->left : current->right;

        if (!parent) {
            // Удаляем корень
            binary_tree_node_destroy(current);
            *root = child; // Новый корень
            return 0;
        }

        if (parent->left == current)
            parent->left = child;
        else
            parent->right = child;

        binary_tree_node_destroy(current);
        return 0;
    }

    // Удаление узла с двумя детьми
    // Ищем минимум в правом поддереве
    BinaryTreeNode* min_parent = current;
    BinaryTreeNode* min_node = current->right;
    while (min_node->left) {
        min_parent = min_node;
        min_node = min_node->left;
    }

    // Копируем ключ и данные
    free(current->key);
    free(current->data);
    current->key = strdup(min_node->key);
    current->data = strdup(min_node->data);

    // Удаляем min_node
    // Поскольку у min_node **нет левого поддерева**, можем просто переподвесить правого ребёнка
    if (min_parent->left == min_node)
        min_parent->left = min_node->right;
    else
        min_parent->right = min_node->right;

    binary_tree_node_destroy(min_node);

    return 0;
}
