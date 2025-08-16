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

Table* core_file_bin_import(Table* table, const char* file_name) {
    if (!table || !file_name) return NULL;

    FILE* f = fopen(file_name, "rb");
    if (!f) return NULL;

    const unsigned int KEY_MAX = 1u << 20; // 1 MiB — ограничение на длину ключа (защита от мусорных/вредных файлов)
    while (1) {
        unsigned int key_len = 0;
        size_t r1 = fread(&key_len, sizeof(unsigned int), 1, f);
        if (r1 == 0) {
            // нормальное завершение по EOF
            break;
        }
        if (r1 != 1) {
            // битая запись/ошибка чтения
            fclose(f);
            return NULL;
        }

        if (key_len == 0 || key_len > KEY_MAX) {
            // невалидная длина
            fclose(f);
            return NULL;
        }

        char* key = (char*)malloc((size_t)key_len + 1);
        if (!key) {
            fclose(f);
            return NULL;
        }

        // читаем key_len байт ключа (без завершающего 0)
        if (fread(key, 1, key_len, f) != key_len) {
            free(key);
            fclose(f);
            return NULL;
        }
        key[key_len] = '\0';

        unsigned int info_u32 = 0;
        if (fread(&info_u32, sizeof(unsigned int), 1, f) != 1) {
            free(key);
            fclose(f);
            return NULL;
        }

        // вставляем (core_insert сам разрулит дубликаты/версии)
        if (core_insert(key, (unsigned int)info_u32, table) == 0) {
            // 0 у тебя значит «не вставлено» (переполнение/коллизия с иным ключом и т.п.)
            // считаем это ошибкой импорта
            free(key);
            fclose(f);
            return NULL;
        }
        free(key);
    }
    fclose(f);
    return table;
}

int core_file_bin_export(const Table* table, const char* file_name) {
    if (!table || !file_name) return -1;

    FILE* f = fopen(file_name, "wb");
    if (!f) return -1;

    for (size_t i = 0; i < table->max_size; ++i) {
        const KeySpace* slot = &table->ks[i];
        if (!slot->busy || !slot->key) continue;

        const char* key = slot->key;
        size_t key_len_sz = strlen(key);
        if (key_len_sz > (size_t)~(unsigned int)0) { /* > UINT_MAX */
            fclose(f); return -1;
        }
        unsigned int key_len = (unsigned int)key_len_sz;

        const Node* cur = slot->node;
        while (cur) {
            unsigned int info = cur->info;
            if (fwrite(&key_len, sizeof(unsigned int), 1, f) != 1)    { fclose(f); return -1; } // записываем длину ключа
            if (key_len > 0 && fwrite(key, 1, key_len, f) != key_len) { fclose(f); return -1; } // записываем ключ
            if (fwrite(&info, sizeof(unsigned int), 1, f) != 1)       { fclose(f); return -1; } // записываем информацию

            cur = cur->next;
        }
    }

    if (fflush(f) != 0) { fclose(f); return -1; }
    fclose(f);
    return 0;
}

unsigned long djb2_hash(const char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash;
}
