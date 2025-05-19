#include "key.h"
#ifndef LIBRARY_H
#define LIBRARY_H

typedef struct Table {
    KeySpace* ks;
    int size;  // лучше использовать size_t     
    int max_size; // лучше использовать size_t        
} Table;

void core_init_table(Table* table, const int initial_size) ;
int core_insert(const char* insert_key,const unsigned int insert_info, Table* table);
int core_binary_search(const char* search_key,Table* table);
int core_delete(const char* delete_key,Table* table);
void core_print_table(Table* table);
void free_table(Table* table);

Table* core_file_import(Table* table,const char* file_name);
Table* core_special_search(const char* start_key, const char* end_key, Table* table);

#endif