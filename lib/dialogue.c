#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include "library.h"
#include "work_library.h"

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

    if (res == -1) {
        printf("Error: Duplicate hash index with different key (collision)\n");
        return 1;
    }
    if (res == 1) {
        printf("Error: Table overflow\n");
        return 1;
    }

    printf("✅ Insert successful\n");
    return 0;
}

int search_key(Table* table)
{
    if (!table) return -1;

    char* search_key = readline("Enter key: ");
    if (!search_key) return -1;

    Node* node = core_search(search_key, table);
    if (!node) {
        printf("❌ Key not found\n");
        free(search_key);
        return 1;
    }

    printf("🔎 Versions for key \"%s\":\n", search_key);
    print_list(node);

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
    core_clear_table(table);
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

    if (!res) {
        printf("❌ Error: File not found or invalid data\n");
        return NULL;
    }

    printf("✅ Table successfully imported\n");
    return res;
