#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include "work_library.h"
#include "lib_binary_tree.h"
#include "dialogue.h"
#include "graphviz_print.h"



BinaryTree* create_binary_tree() {
    BinaryTree* tree = binary_tree_create();
    if (!tree) {
        printf("Error: Failed to create binary tree\n");
        return NULL;
    }
    printf("Binary tree created successfully\n");
    return tree;
}
int insert_node_to_binary_tree(BinaryTree* tree) {
    if (!tree) {
        printf("Error: Binary tree is null\n");
        return -1;
    }

    char* key = readline("Enter key: ");
    if (!key) return -1;

    char* data = readline("Enter data: ");
    if (!data) {
        free(key);
        return -1;
    }

    int res = binary_tree_insert(tree, key, data);
    free(key);
    free(data);

    if (res == -1) {
        printf("Error: Failed to insert node into binary tree\n");
        return -1;
    }

    printf("✅ Node inserted successfully\n");
    return 0;
}

int search_key_in_binary_tree(BinaryTree* tree) {
    if (!tree) {
        printf("Error: Binary tree is null\n");
        return -1;
    }

    char* key = readline("Enter key: ");
    if (!key) return -1;

    BinaryTreeNode* node = binary_tree_search(tree, key);
    free(key);

    if (!node) {
        printf("❌ Key not found in binary tree\n");
        return 1;
    }

    printf("🔎 Node found: key='%s', data='%s'\n", node->key, node->data);
    return 0;
}

int delete_node_in_binary_tree(BinaryTree* tree) {
    if (!tree) {
        printf("Error: Binary tree is null\n");
        return -1;
    }

    char* key = readline("Enter key to delete: ");
    if (!key) return -1;

    int res = binary_tree_erase_node_by_key(tree, key);
    if (res == 1) {
        printf("❌ Key not found in binary tree\n");
    } else  {
        printf("✅ Node deleted successfully\n");
    } 
    free(key);
    return (-1)*res;
}
int clear_binary_tree(BinaryTree* tree) {
    if (!tree) {
        printf("Error: Binary tree is null\n");
        return -1;
    }

    binary_tree_destroy(&tree);
    if (tree) {
        printf("❌ Failed to clear binary tree\n");
        return -1;
    }
    printf("✅ Binary tree cleared\n");
    return 0;
}

void print_binary_tree(BinaryTree* tree) {
    if(!tree) {
        printf("❌ Binary tree is not initialized\n");
        return;
    }
    binary_tree_print_sort_view(tree);
    printf("Binary tree printed successfully\n");
}

void print_tree_view_binary_tree(BinaryTree* tree) {
    if(!tree) {
        printf("❌ Binary tree is not initialized\n");
        return;
    }
    print_binary_tree(tree);
    printf("Binary tree printed successfully\n");
}

BinaryTree* binary_tree_file_import(int* eof_tmp) {
    char* file_name = readline("Enter file name: ");
    if (!file_name) {
        *eof_tmp = -1;
        return NULL;
    }

    BinaryTree* tree = binary_tree_create_from_file(file_name);
    free(file_name);
    
    if (tree == NULL) {
        printf("Error: File not found/invalid data entry\n");
        return NULL;
    }
    return tree;
}

int special_search_in_binary_tree(BinaryTree* tree) {
    if (!tree) {
        printf("Error: Binary tree is null\n");
        return -1;
    }

    char* key = readline("Enter the search key: ");
    if (!key) return -1;

    KeyArray* array = binary_tree_get_all_nodes_max_diff_symbols(tree, key);
    free(key);
    if(!array) {
        printf("❌ No nodes found with the specified key\n");
        return 1;
    }
    printf("✅ Nodes found with the specified key:\n");
    key_array_print(array);
    key_array_delete(array);
    printf("✅ Search completed successfully\n");
    return 0;
}
int generate_graphviz_file(const BinaryTree *tree)
{
    if (!tree) {
        printf("Error: Binary tree is null\n");
        return -1;
    }
    char* filename = readline("Enter output filename for Graphviz: ");
    if (!filename) {
        printf("Error: Failed to read filename\n");
        return -1;
    }
    binary_tree_generate_dot(tree, filename);
    printf("You can view it with command: dot -Tpng -o output.png %s\n", filename);
    free(filename);
    printf("✅ Graphviz file generated successfully\n");
    
    return 0;
}