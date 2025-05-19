#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "library.h"
#include "work_library.h"
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

Table* core_init_table(Table* table,size_t initial_size) 
{
  if(table != NULL) {
    table->ks = malloc(initial_size * sizeof(KeySpace));
    if (!table->ks) {
        free(table);
        return NULL;
    }
    table->size = 0;
    table->max_size = initial_size;
  }
  return table;
}

int core_insert(const char* insert_key,unsigned int insert_info, Table* table)
{
    if(!table || !insert_key || table->size < 0) {
        printf("core_insert error: table is NULL\n");
        return -1; //ÐžÑˆÐ¸Ð±ÐºÐ° Ð² Ð²Ñ…Ð¾Ð´Ð½Ñ‹Ñ… Ð´Ð°Ð½Ð½Ñ‹Ñ…
    }
    if(table->size == table->max_size) 
        return 1; // переполнение таблицы
    
    if(core_binary_search(insert_key,table) != -1) 
        return -1; // данные с таким ключом уже существуют
    
    
    int insert_index = table->size; // потенциально опасный индекс, может быть 0
    while(insert_index > 0 && strcmp(table->ks[insert_index-1].key, insert_key) > 0)
    {
        table->ks[insert_index].key = table->ks[insert_index-1].key;
        table->ks[insert_index].info = table->ks[insert_index-1].info;
        insert_index--; 
    }
    if( keyspace_create(&table->ks[insert_index], insert_key, insert_info) == NULL) {
        return -1; // ошибка создания ключа
    }
    table->size++;
        
    return 0;
}

typedef enum {
    BSEARCH_EXACT_MATCH = 0,
    BSEARCH_INSERT_LEFT
} BinarySearchMode;

int core_binary_search_internal(const char* key, Table* table, BinarySearchMode mode) {
    int left = 0;
    int right = table->size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcmp(table->ks[mid].key, key);

             if (cmp == 0) { return  mid; }     // Найден точный ключ
        else if (cmp  < 0) { left  = mid + 1; } // Ищем в правой половине
        else               { right = mid - 1; } // Ищем в левой половине
        
    }

    return (mode == BSEARCH_INSERT_LEFT) ? left : -1; // Возвращаем позицию для вставки или -1
}

int core_binary_search(const char* search_key,Table* table) {
    return core_binary_search_internal(search_key, table, BSEARCH_EXACT_MATCH);
}

int core_binary_left(const char* search_key,Table* table) {
    return core_binary_search_internal(search_key, table, BSEARCH_INSERT_LEFT);
}

//Ð£Ð´Ð°Ð»ÐµÐ½Ð¸Ðµ
int core_delete(const char* delete_key,Table* table)
{
    int found = core_binary_search(delete_key,table);
    if(found == -1) 
        return -1;// key not found
    
    keyspace_destroy(&table->ks[found]);
    table->size -= 1;
    for(int i = found; i < table->size - 1; i++)
    {
        table->ks[i] = table->ks[i + 1];
    }
    
    return 0;
}

//Ð’Ñ‹Ð²Ð¾Ð´ Ð² ÐºÐ¾Ð½ÑÐ¾Ð»ÑŒ
void core_print_table(Table* table)
{
    if (!table) return;

    printf("---Table output---\nSize:%d,\nMaxSize:%d\n",table->size,table->max_size);
    for (int i = 0; i < table->size; i++) {
        printf("|Key:%s|Info:%p|\n",table->ks[i].key,table->ks[i].info);
    }

}

//ÐžÑÐ²Ð¾Ð±Ð¾Ð¶Ð´ÐµÐ½Ð¸Ðµ Ð¿Ð°Ð¼ÑÑ‚Ð¸
void free_table(Table* table) 
{
    free_partial_table(table, table->size);      
}

Table* core_file_import(Table* table,const char* file_name)
{
    FILE *file = fopen(file_name, "r");
    if(file == NULL)
    {
        return NULL;
    }
    char* str;
    while((str = getstr(file)) != NULL)
    {
        char* key = strtok(str, " ");
        char* info_str = strtok(NULL, " ");
        if (key && info_str) {
            unsigned int info = atoi(info_str);
            core_insert(key, info, table);
        }
        free(str); 
    }
    fclose(file);
    return table;
    
}

Table* core_range_search(const char* start_key, const char* end_key, Table* table) 
{
    if (!table || !start_key || !end_key || table->size == 0 || strcmp(start_key, end_key) > 0)
        return NULL;
   
    int start_index = core_binary_left(start_key, table);
    if(strcmp(table->ks[start_index].key, start_key) <= 0)
        start_index++;

    int end_index = core_binary_left(end_key, table);

    if (start_index > end_index)
        return NULL;

    int count = end_index - start_index;

    Table* new_table = malloc(sizeof(Table));
    if (!new_table)
        return NULL;

    if(core_init_table(new_table, count) == NULL) 
        return NULL;
    
    new_table->size = count;
    
    for (int i = 0; i < count; i++) {
        const KeySpace* src = &table->ks[start_index + i];
        KeySpace* dst = &new_table->ks[i];

        if (keyspace_create(dst, src->key, src->info) == NULL) {
            free_partial_table(new_table, i);
            return NULL;
        }
    }

    return new_table;
}


void free_partial_table(Table* table, int filled_count) {
    if (!table) return;

    for (int i = 0; i < filled_count; i++)
        keyspace_destroy(&table->ks[i]);

    free(table->ks);
    free(table);
}
