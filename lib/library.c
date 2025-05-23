#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "library.h"
#include "work_library.h"

Table* core_init_table(Table* table, size_t initial_size) {
    if (table) {
        table->ks = malloc(initial_size * sizeof(KeySpace));
        if (!table->ks) {
            free(table);
            return NULL;
        }
        table->size = 0;
        table->max_size = initial_size;

        for (size_t i = 0; i < initial_size; i++) {
            table->ks[i].busy = 0;
            table->ks[i].list_length = 0;
            table->ks[i].key = NULL;
            table->ks[i].node = NULL;
        }
    }
    return table;
}

int core_insert(const char* insert_key, unsigned int insert_info, Table* table) {
    if (!table || !insert_key) return 0;
    if (table->size == table->max_size) return 0;  // переполнение

    unsigned long hash = djb2_hash(insert_key);
    size_t index = hash % table->max_size;

    KeySpace* slot = &table->ks[index];

    return add_node(slot, insert_key, insert_info, table);
    
    // Коллизия — разные ключи по одному индексу
    // fprintf(stderr, "core_insert error: collision at index %zu (keys '%s' vs '%s')\n",
    //         index, slot->key, insert_key);
}

RelType add_node(KeySpace *slot, const char *insert_key, unsigned int insert_info, Table *table)
{
    if (!slot->busy) {
        // Пустая ячейка — вставляем
        slot->busy = 1;
        slot->key = strdup(insert_key);
        table->size++;
    } else if (strcmp(slot->key, insert_key) != 0) { // Уже есть данные — проверим ключ
        return 0;
    }

    RelType release = 0;
    slot->node = node_insert(slot->node, insert_info, &release);
    if(release > 0)
        slot->list_length++;

    return release;
}

//Ð£Ð´Ð°Ð»ÐµÐ½Ð¸Ðµ
int core_delete(const char* key, Table* table) {
    if (!table || !key) return -1;

    unsigned long hash = djb2_hash(key);
    size_t index = hash % table->max_size;

    KeySpace* slot = &table->ks[index];

    if (slot->busy && strcmp(slot->key, key) == 0) {
        free(slot->key);
        node_destroy(slot->node);
        slot->key = NULL;
        slot->node = NULL;
        slot->busy = 0;
        slot->list_length = 0;
        table->size--;
        return 0;
    }

    return -1; // not found
}

KeySpace* core_search(const char* key, const Table* table) {
    if (!table || !key || table->max_size == 0)
        return NULL;

    unsigned long hash = djb2_hash(key);
    size_t index = hash % table->max_size;

    KeySpace* slot = &table->ks[index];

    if (slot->busy && slot->key && strcmp(slot->key, key) == 0) {
        return slot;
    }

    return NULL;  // либо пусто, либо другой ключ (коллизия)
}

Node* core_search_by_key_and_release(const char* key, RelType release, const Table* table) {
    KeySpace* slot = NULL;
    slot = core_search(key, table);
    if (slot != NULL && slot->node != NULL) {
        return node_find(slot->node, release);
    }

    return NULL;
}

int core_delete_by_key_and_release(const char* key, RelType release, Table* table) {
    if (!table || !key || table->max_size == 0)
        return -1;

    unsigned long hash = djb2_hash(key);
    size_t index = hash % table->max_size;

    KeySpace* slot = &table->ks[index];
    if (slot->busy && strcmp(slot->key, key) == 0) {
        int status = node_delete(&slot->node, release);
        slot->list_length--;
        if (slot->node == NULL) {
            // Если это была последняя версия — удаляем и KeySpace
            free(slot->key);
            slot->key = NULL;
            slot->busy = 0;
            slot->list_length = 0;
            table->size--;
        }

        return status; // 0 - удалено, -1 - не найдено
    }

    return -1;
}

//Ð’Ñ‹Ð²Ð¾Ð´ Ð² ÐºÐ¾Ð½ÑÐ¾Ð»ÑŒ
void core_print_table(Table* table)
{
    if (!table) return;

    printf("---Table output---\nSize:%ld,\nMaxSize:%ld\n",table->size,table->max_size);
    for (size_t i = 0; i < table->max_size; ++i) {
        const KeySpace* slot = &table->ks[i];
        if (slot->busy && slot->key) {
            printf("Index %zu: key = \"%s\"\n", i, slot->key);
            print_list(slot->node);
        }
    }

}

void free_table(Table* table) 
{
    if (!table || !table->ks || table->max_size == 0)
        return;

    for (size_t i = 0; i < table->max_size; ++i) {
        KeySpace* slot = &table->ks[i];

        if (slot->busy) {
            free(slot->key);
            node_destroy(slot->node);

            slot->key = NULL;
            slot->node = NULL;
            slot->busy = 0;
        }
    }

    table->size = 0;
    free(table->ks);
    free(table);   
}

Table* core_file_import(Table* table, const char* file_name) {
    if (!table || !file_name) return NULL;

    FILE* file = fopen(file_name, "r");
    if (!file) return NULL;

    char* str;
    while ((str = getstr(file)) != NULL) {
        char* key = strtok(str, " ");
        char* info_str = strtok(NULL, " ");

        if (key && info_str) {
            unsigned int info = (unsigned int)atoi(info_str);
            int status = core_insert(key, info, table);
            if (status == -1) {
                fprintf(stderr, "⚠️ Ошибка вставки ключа \"%s\"\n", key);
            }
        }
        free(str);
    }

    fclose(file);
    return table;
}


unsigned long djb2_hash(const char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash;
}
