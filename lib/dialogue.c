#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include "dialogue.h"
#include "graphviz_print.h"

Node234* create_234_tree() {
    Node234* root = node234_create_node(0, NULL);
    if (!root) {
        printf("Error: Failed to create 2-3-4 tree\n");
        return NULL;
    }
    printf("2-3-4 tree created successfully\n");
    return root;
}

int insert_node_to_234_tree(Node234** root_ref) {
    char* key = readline("Enter key: ");
    if (!key) return -1;
    char* value = readline("Enter value: ");
    if (!value) {
        free(key);
        return -1;
    }

    node234_insert(root_ref, key, value);
    free(key);
    free(value);
    printf("\u2705 Node inserted successfully\n");
    return 0;
}

int search_key_in_234_tree(Node234* root) {
    char* key = readline("Enter key: ");
    if (!key) return -1;
    Element* el = node234_search_by_key(root, key);
    free(key);

    if (!el) {
        printf("\u274C Key not found in tree\n");
        return 1;
    }
    printf("\U0001F50D Found: key='%s', value='%s'\n", el->key, el->value);
    return 0;
}

int delete_node_in_234_tree(Node234** root_ref) {
    char* key = readline("Enter key to delete: ");
    if (!key) return -1;

    int res = node234_delete_by_key(root_ref, key);
    if (res == 1) printf("\u2705 Node deleted successfully\n");
    else printf("\u274C Key not found in tree\n");

    free(key);
    return res ? 0 : -1;
}

int clear_234_tree(Node234** root_ref) {
    if (!*root_ref) return -1;
    node234_destroy(*root_ref);
    *root_ref = NULL;
    printf("\u2705 Tree cleared\n");
    return 0;
}

void print_tree_view_234_tree(Node234* root) {
    node234_tree_print(root, 0);
    printf("Tree printed successfully\n");
}

void print_sort_view_234_tree(Node234* root) {
    node234_print_sort_view(root);
    printf("\n\u2705 Tree printed in sorted order\n");
}

int special_search_in_234_tree(Node234* root) {
    

    Element result = node234_find_max(root);
    if (result.key == NULL) {
        printf("\u274C No elements found in the tree\n");
        return -1;
    }
    printf("\U0001F50D Max element found: key='%s', value='%s'\n", result.key, result.value);
   
    return 0;
}

int generate_graphviz_file_234_tree(const Node234* root) {
    char* filename = readline("Enter filename for Graphviz output: ");
    if (!filename) return -1;
    node234_generate_dot(root, filename);
    printf("Use command: dot -Tpng -o out.png %s\n", filename);
    free(filename);
    return 0;
}

Node234* node_234_tree_file_import(int* eof_tmp) {
    char* file_name = readline("Enter file name: ");
    if (!file_name) {
        *eof_tmp = -1;
        return NULL;
    }


    Node234* root = node234_create_from_file(file_name);

    free(file_name);
    if (!root) {
        *eof_tmp = -1;
        printf("Error: Failed to import tree from file\n");
        return NULL;
    }
    printf("✅ Tree imported from file\n");
    return root;
}
