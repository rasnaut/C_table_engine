#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib_tree_234.h"

Node234* node234_create_node(int child_count, Node234* parent) {
    Node234* node = (Node234*)malloc(sizeof(Node234));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for Node234\n");
        exit(EXIT_FAILURE);
    }
    node->key_count = 0;
    node->child_count = child_count;
    node->parent = parent;
    for (int i = 0; i < MAX_CHILDREN; ++i) { 
        node->children[i] = NULL;
    }
    for (int i = 0; i < MAX_KEYS; ++i) {
        node->elements[i].key = NULL;
        node->elements[i].value = NULL;
    }
    return node;
}

void node234_delete_node(Node234* node) {
    if (!node) return;
    for (int i = 0; i < node->key_count; ++i) {
        free(node->elements[i].key);
        free(node->elements[i].value);
    }
    free(node);
}

void node234_add_child(Node234 *parent, Node234 *child)
{
    if (parent->child_count >= MAX_CHILDREN) {
        fprintf(stderr, "Error: Cannot add child, maximum children limit reached\n");
        return;
    }
    parent->children[parent->child_count] = child;
    child->parent = parent;
    parent->child_count++;
}

void split_child(Node234 *parent, int child_index, Node234 *child) {
    // Новый узел для правой половины
    Node234* new_node = node234_create_node(0, parent);
    new_node->key_count = 1;
    new_node->elements[0] = child->elements[2];

    // Копируем потомков, если они есть
    if (child->child_count > 0) {
        node234_add_child(new_node, child->children[2]);
        node234_add_child(new_node, child->children[3]);
        child->children[2] = NULL;
        child->children[3] = NULL;
        child->child_count = 2;
    }

    // Сохраняем только первую половину у исходного узла
    child->key_count = 1;

    // Сдвигаем детей и ключи родителя, чтобы вставить новый элемент
    for (int j = parent->key_count; j > child_index; j--) {
        parent->children[j+1] = parent->children[j];
        parent->children[j+1]->parent = parent;
        parent->elements[j] = parent->elements[j-1];
    }

    parent->children[child_index + 1] = new_node;
    new_node->parent = parent;
   
    parent->elements[child_index] = child->elements[1];
    child->parent = parent;
    parent->key_count++;
    parent->child_count++;
}

void insert_nonfull(Node234* node, const char* key, const char* value) {
    Node234* current = node;

    while(1) {
        int key_index = current->key_count - 1;
        if (current->child_count == 0) {
            // Insert into leaf node
            while (key_index >= 0 && strcmp(current->elements[key_index].key, key) > 0) {
                current->elements[key_index + 1] = current->elements[key_index];
                key_index--;
            }
            current->elements[key_index + 1].key = strdup(key);
            current->elements[key_index + 1].value = strdup(value);
            current->key_count++;
            return;
        } else {
            while (key_index >= 0 && strcmp(key, current->elements[key_index].key) < 0) key_index--;
            
            key_index++;
            if (current->children[key_index]->key_count == MAX_KEYS) {
                split_child(current, key_index, current->children[key_index]);
                if (strcmp(key, current->elements[key_index].key) > 0) key_index++;
            }
            current = current->children[key_index];
        }
    }
}

void insert(Node234** root_ref, const char* key, const char* value) {
    Node234* root = *root_ref;
    if (root->key_count == MAX_KEYS) {
        Node234* new_root = node234_create_node(0, NULL);
        new_root->children[new_root->child_count] = root;
        new_root->child_count++;
        split_child(new_root, 0, root);
        insert_nonfull(new_root, key, value);
        *root_ref = new_root;
    } else {
        insert_nonfull(root, key, value);
    }
}

// Simple traversal to visualize
void node234_tree_print(Node234* node, int level) {
    if (!node) return;
    for (int i = 0; i < level; ++i) printf("  ");
    printf("[");
    for (int i = 0; i < node->key_count; ++i) {
        printf("%s", node->elements[i].key);
        if (i != node->key_count - 1) printf(", ");
    }
    printf("]\n");
    for (int i = 0; i < node->child_count; ++i)
        node234_tree_print(node->children[i], level + 1);
}

Element* node234_search_by_key(Node234* root, const char* key) {
    printf("Searching for key: %s\n", key);
    Node234* current = root;

    while (current) {
        int key_index = 0;
        // Ищем первый ключ больше либо равный искомому
        while (key_index < current->key_count && strcmp(key, current->elements[key_index].key) > 0)
            key_index++;
            
        // Нашли точное совпадение
        if (key_index < current->key_count && strcmp(key, current->elements[key_index].key) == 0)
            return &current->elements[key_index];

        if (!current->child_count) 
            return NULL; // Если это лист, больше некуда идти

        current = current->children[key_index]; // Иначе спускаемся к следующему ребёнку
    }
    return NULL;
}

KeyArray* node234_get_all_nodes_contained_substring(Node234* root, const char* substring) {
    if (!root || !substring) return NULL;

    KeyArray* result = key_array_create();
    if (!result) {
        fprintf(stderr, "Error: Could not create KeyArray\n");
        return NULL;
    }

    Node234* stack[128];
    int stack_top = -1;
    Node234* node = root;

    while (node || stack_top >= 0) {
        if (node) {
            // Проверяем все ключи узла
            for (int i = 0; i < node->key_count; i++) {
                if (strncmp(node->elements[i].key, substring, strlen(substring)) == 0)
                    key_array_push_back(result, node->elements[i].key);
            }
            // Спускаемся влево
            if (node->child_count) {
                stack[++stack_top] = node;
                node = node->children[0];
            } else {
                node = NULL;
            }
        } else {
            // Поднимаемся
            node = stack[stack_top--];
            int child_index = 1;
            if (child_index <= node->key_count) {
                node = node->children[child_index++];
            } else {
                node = NULL;
                child_index = 1;
            }
        }
    }
    return result;
}

KeyArray* node234_get_all_nodes_max_diff_symbols(const Node234* root, const char* target_key) {
    if (!root || !target_key) return NULL;

    KeyArray* result = key_array_create();
    if (!result) {
        fprintf(stderr, "Error: Could not create KeyArray\n");
        return NULL;
    }

    const Node234* stack[128];
    int stack_top = -1;
    const Node234* node = root;

    while (node || stack_top >= 0) {
        if (node) {
            for (int i = 0; i < node->key_count; i++) {
                int has_common = 0;
                for (const char* p = node->elements[i].key; *p; ++p) {
                    if (strchr(target_key, *p)) {
                        has_common = 1;
                        break;
                    }
                }
                if (!has_common)
                    key_array_push_back(result, node->elements[i].key);
            }

            if (node->child_count) {
                stack[++stack_top] = node;
                node = node->children[0];
            } else {
                node = NULL;
            }
        } else {
            node = stack[stack_top--];
            int child_index = 1;
            if (child_index <= node->key_count) {
                node = node->children[child_index++];
            } else {
                node = NULL;
                child_index = 1;
            }
        }
    }
    return result;
}

void node234_print_sort_view(const Node234* root) {
    if (!root) return;

    const Node234* stack[128];
    int stack_top = -1;
    const Node234* node = root;
    int child_index = 0;

    while (node || stack_top >= 0) {
        if (node) {
            if (node->child_count) {
                stack[++stack_top] = node;
                node = node->children[0];
                child_index = 0;
            } else {
                for (int i = 0; i < node->key_count; i++)
                    printf("%s ", node->elements[i].key);
                node = NULL;
            }
        } else {
            node = stack[stack_top--];

            if (child_index < node->key_count) {
                printf("%s ", node->elements[child_index].key);
                node = node->children[++child_index];
            } else {
                node = NULL;
            }
        }
    }
}

void node234_destroy(Node234* root) {
    if (!root) return;

    Node234* node = root;

    while (node) {
        if(node->child_count > 0) { 
            node = node->children[node->child_count-1]; 
        } else {
            Node234* parent = node->parent;
            node234_delete_node(node);
            node = parent;
            if(node) {
                node->children[node->child_count - 1] = NULL;
                node->child_count--;
            } 
        }
    }
}


int node234_delete_by_key(Node234 **root_ref, const char *key) {
    Node234 *root = *root_ref;
    Node234 *node = root;

    while (node) {
        int i = 0;
        // Найдём позицию ключа (или ребёнка для спуска)
        while (i < node->key_count && strcmp(key, node->elements[i].key) > 0)
            i++;

        if (i < node->key_count && strcmp(key, node->elements[i].key) == 0) {
            // Ключ найден
            if (node->child_count == 0) {
                // В листе — просто удалить
                free(node->elements[i].key);
                free(node->elements[i].value);
                for (int j = i; j < node->key_count - 1; ++j)
                    node->elements[j] = node->elements[j + 1];
                node->key_count--;
                break;
            } else {
                // Внутренний узел — заменяем на минимальный в правом поддереве
                Node234 *succ = node->children[i + 1];
                while (succ->child_count)
                    succ = succ->children[0];
                // Копируем ключ/значение преемника
                free(node->elements[i].key);
                free(node->elements[i].value);
                node->elements[i].key = strdup(succ->elements[0].key);
                node->elements[i].value = strdup(succ->elements[0].value);
                // Переходим к удалению этого ключа в листе
                node = node->children[i + 1];
                key = succ->elements[0].key;
                i = 0;
            }
        }

        if (node->child_count == 0) {
            // Ключ не найден — выходим
            return 0;
        }

        // Гарантируем, что в child[i] минимум 2 ключа
        Node234 *child = node->children[i];
        if (child->key_count == 1) {
            Node234 *left_sibling = (i > 0) ? node->children[i - 1] : NULL;
            Node234 *right_sibling = (i < node->key_count) ? node->children[i + 1] : NULL;

            if (left_sibling && left_sibling->key_count > 1) {
                // Перенос ключа слева
                for (int j = child->key_count; j > 0; --j)
                    child->elements[j] = child->elements[j - 1];
                if (child->child_count > 0) {
                    for (int j = child->child_count; j > 0; --j)
                        child->children[j] = child->children[j - 1];
                }
                child->elements[0] = node->elements[i - 1];
                if (child->child_count > 0)
                    child->children[0] = left_sibling->children[left_sibling->child_count - 1];
                child->key_count++;

                node->elements[i - 1] = left_sibling->elements[left_sibling->key_count - 1];
                left_sibling->key_count--;
                if (left_sibling->child_count > 0)
                    left_sibling->child_count--;
            } else if (right_sibling && right_sibling->key_count > 1) {
                // Перенос ключа справа
                child->elements[child->key_count] = node->elements[i];
                if (child->child_count > 0)
                    child->children[child->child_count] = right_sibling->children[0];
                child->key_count++;
                if (child->child_count > 0)
                    child->child_count++;

                node->elements[i] = right_sibling->elements[0];
                for (int j = 0; j < right_sibling->key_count - 1; ++j)
                    right_sibling->elements[j] = right_sibling->elements[j + 1];
                if (right_sibling->child_count > 0) {
                    for (int j = 0; j < right_sibling->child_count - 1; ++j)
                        right_sibling->children[j] = right_sibling->children[j + 1];
                    right_sibling->child_count--;
                }
                right_sibling->key_count--;
            } else {
                // Слияние
                if (left_sibling) {
                    // Слияние с левым братом
                    left_sibling->elements[left_sibling->key_count] = node->elements[i - 1];
                    left_sibling->key_count++;
                    for (int j = 0; j < child->key_count; ++j)
                        left_sibling->elements[left_sibling->key_count++] = child->elements[j];
                    for (int j = 0; j < child->child_count; ++j)
                        left_sibling->children[left_sibling->child_count++] = child->children[j];
                    for (int j = i - 1; j < node->key_count - 1; ++j)
                        node->elements[j] = node->elements[j + 1];
                    for (int j = i; j < node->child_count - 1; ++j)
                        node->children[j] = node->children[j + 1];
                    node->key_count--;
                    node->child_count--;
                    node234_delete_node(child);
                    child = left_sibling;
                } else if (right_sibling) {
                    // Слияние с правым братом
                    child->elements[child->key_count++] = node->elements[i];
                    for (int j = 0; j < right_sibling->key_count; ++j)
                        child->elements[child->key_count++] = right_sibling->elements[j];
                    for (int j = 0; j < right_sibling->child_count; ++j)
                        child->children[child->child_count++] = right_sibling->children[j];
                    for (int j = i; j < node->key_count - 1; ++j)
                        node->elements[j] = node->elements[j + 1];
                    for (int j = i + 1; j < node->child_count - 1; ++j)
                        node->children[j] = node->children[j + 1];
                    node->key_count--;
                    node->child_count--;
                    node234_delete_node(right_sibling);
                }
            }
        }

        // Спускаемся дальше
        node = child;
    }

    // После удаления — обновляем root, если он пустой
    if (root->key_count == 0 && root->child_count > 0) {
        *root_ref = root->children[0];
        (*root_ref)->parent = NULL;
        node234_delete_node(root);
    }
    return 1;
}
