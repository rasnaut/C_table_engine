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
    
    core_init_table(table, initial_size);
    return 0;
} 

int insert(Table* table)
{
    if(table == NULL) {
        printf("Error: Table is null\n");
        return 1;
    }

    char* insert_key = readline("Enter key: ");
    if (!insert_key) return -1;
    
    char* info_input = readline("Enter information:");
    if (!info_input) 
    {
        free(insert_key);
        return -1;
    }
    
    int insert_info = atoi(info_input); // будет 0 при пустой строке
    free(info_input);
    
    int res = core_insert(insert_key, insert_info, table);
    free(insert_key);
    
    if (res == -1) 
    {
        printf("Error: Duplicate keys\n");
        return 1;
    }
    if (res == 1) 
    {
        printf("Error: No available positions\n");
        return 1;
    }
    return 0;
}

int binary_search(Table* table)
{
    char* search_key = readline("Enter key: ");
    if (!search_key) return -1;
    
    int res = core_binary_search(search_key, table);
    free(search_key);
    
    if (res == -1) 
    {
        printf("This key was not found\n");
        return 1;
    }
    printf("Found key:%s\n", table->ks[res].key);
    printf("Index:%d\n", res);
    return 0;   
}

int T_delete(Table* table)
{
    char* delete_key = readline("Enter key: ");
    if (!delete_key) return -1;
    
    int res = core_delete(delete_key, table);
    free(delete_key);
    
    if (res == -1) 
    {
        printf("Item not found\n");
        return -1;
    }
    return 0;
}

void print_table(Table* table) {
    if (!table) {
        printf("The table is not initialized\n");
        return;
    }
    core_print_table(table);
}

Table* file_import(Table* table, int* eof_tmp)
{
    char* file_name = readline("Enter file name:");
    if (!file_name) 
    {
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

Table* special_search(Table* table, int* eof_tmp)
{
    char* start_key = readline("Enter the initial key: ");
    if (!start_key) 
    {
        *eof_tmp = -1;
        return NULL;
    }
    
    char* end_key = readline("Enter the final key: ");
    if (!end_key) 
    {
        free(start_key);
        *eof_tmp = -1;
        return NULL;
    }
    
    Table* res = core_range_search(start_key, end_key, table);
    free(start_key);
    free(end_key);
    
    if (res == NULL) 
    {
        printf("Error: Invalid range entered/memory allocated\n");
        return NULL;
    }
    return res;
}