#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include "library.h"
#include "work_library.h"
#include "lib_binary_tree.h"
#include "dialogue.h"

int init_table(Table* table)
{
    char* input = readline("Enter table size: ");
    if (!input) return -1;

    int initial_size = atoi(input);
    free(input);

    if (initial_size <= 0) return -1;

    if (core_init_table(table, initial_size) == NULL) {
        printf("Failed to initialize table\n");
        return -1;
    }

    return 0;
}

int insert(Table* table)
{
    if (!table) {
        printf("Error: Table is null\n");
        return 1;
    }

    char* insert_key = readline("Enter key: ");
    if (!insert_key) return -1;

    char* info_input = readline("Enter information: ");
    if (!info_input) {
        free(insert_key);
        return -1;
    }

    int insert_info = atoi(info_input);
    free(info_input);

    int res = core_insert(insert_key, insert_info, table);
    free(insert_key);

    if (!res) {
        printf("Error: inser error\n");
        return 1;
    }
    

    printf("✅ Insert successful insert by release: %d\n", res);
    return 0;
}

int search_key(Table* table)
{
    if (!table) return -1;

    char* search_key = readline("Enter key: ");
    if (!search_key) return -1;

    KeySpace* key = core_search(search_key, table);
    if (!key) {
        printf("❌ Key not found\n");
        free(search_key);
        return 1;
    }

    printf("🔎 Versions for key \"%s\":\n", search_key);
    print_list(key->node);

    free(search_key);
    return 0;
}

int T_delete(Table* table)
{
    if (!table) return -1;

    char* key = readline("Enter key: ");
    if (!key) return -1;

    char* ver_input = readline("Enter version (release number): ");
    if (!ver_input) {
        free(key);
        return -1;
    }

    RelType version = (RelType)atoi(ver_input);
    free(ver_input);

    int res = core_delete_by_key_and_release(key, version, table);
    free(key);

    if (res == -1) {
        printf("❌ Element not found or version does not exist\n");
        return -1;
    }

    printf("✅ Version successfully deleted\n");
    return 0;
}

void print_table(Table* table) {
    if (!table) {
        printf("❌ Table is not initialized\n");
        return;
    }
    core_print_table(table);
}

int clear_table(Table* table)
{
    if (!table) return -1;
    free_table(table);
    printf("✅ Table cleared\n");
    return 0;
}

Table* file_import(Table* table, int* eof_tmp)
{
    char* file_name = readline("Enter file name: ");
    if (!file_name) {
        *eof_tmp = -1;
        return NULL;
    }

    Table* res = core_file_import(table, file_name);
    free(file_name);
    
    if (res == NULL) 
    {
        printf("Error: File not found/invalid data entry\n");
        return NULL;
    }
    return res;
}

int special_search(Table* table)
{
    char* search_key = readline("Enter the search key: ");
    if (!search_key) 
    {
        return 1;
    }

    KeySpace* key = core_search(search_key, table);
    if (!key) {
        printf("❌ Key not found\n");
        free(search_key);
        return 1;
    }

    char* input = readline("Enter release number: ");
    if (!input) return -1;

    RelType release = atoi(input);
    free(input);

    Node* find = node_find(key->node, release);
    if (!find) {
        printf("❌ Release %d not exist for key: %s\n", release, key->key);
    }

    printf("✅ Release %d found for key: %s\n", release, key->key);
    return 0;
}


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

    binary_tree_destroy(tree);
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
    free(filename);
    printf("✅ Graphviz file generated successfully\n");
    printf("You can view it with command: dot -Tpng -o output.png %s\n", filename);
    return 0;
}