#include "key.h"
#ifndef LIBRARY_H
#define LIBRARY_H

typedef struct Table {
    KeySpace* ks;
    size_t size;  // лучше использовать size_t     
    size_t max_size; // лучше использовать size_t        
} Table;

Table* core_init_table(Table* table, const size_t initial_size);
int core_insert(const char *insert_key, const unsigned int insert_info, Table *table);

int add_node(KeySpace *slot, const char *insert_key, unsigned int insert_info, Table *table);

int core_delete(const char* delete_key,Table* table);
void core_print_table(Table* table);

KeySpace* core_search(const char* key, const Table* table);
Node* core_search_by_key_and_release(const char* key, RelType release, const Table* table);
int core_delete_by_key_and_release(const char* key, RelType release, Table* table);


void free_table(Table* table);

unsigned long djb2_hash(const char* str); // Hash function


Table* core_file_import(Table* table,const char* file_name);

#endif