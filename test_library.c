#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "library.h"
#include "work_library.h"

void test_init_and_insert() {
    Table* table = malloc(sizeof(Table));
    assert(table != NULL);

    assert(core_init_table(table, 10) != NULL);
    assert(table->size == 0);
    assert(table->max_size == 10);

    assert(core_insert("alpha", 42, table) == 0);
    assert(table->size == 1);
    assert(strcmp(table->ks[0].key, "alpha") == 0);
    assert(table->ks[0].info == 42);

    assert(core_insert("beta", 84, table) == 0);
    assert(table->size == 2);
    assert(strcmp(table->ks[1].key, "beta") == 0);
    assert(table->ks[1].info == 84);

    // вставка дубликата
    assert(core_insert("alpha", 99, table) == -1);

    free_table(table);
}

void test_search_and_delete() {
    Table* table = malloc(sizeof(Table));
    assert(core_init_table(table, 5) != NULL);

    core_insert("a", 1, table);
    core_insert("b", 2, table);
    core_insert("c", 3, table);

    int idx = core_binary_search("b", table);
    assert(idx != -1);
    assert(strcmp(table->ks[idx].key, "b") == 0);

    // Удалим и проверим, что его больше нет
    assert(core_delete("b", table) == 0);
    assert(core_binary_search("b", table) == -1);
    assert(table->size == 2);

    // Удаление несуществующего
    assert(core_delete("zzz", table) == -1);

    free_table(table);
}

void test_range_search() {
    Table* table = malloc(sizeof(Table));
    core_init_table(table, 10);

    core_insert("apple", 1, table);
    core_insert("banana", 2, table);
    core_insert("carrot", 3, table);
    core_insert("date", 4, table);
    core_insert("fig", 5, table);

    Table* sub = core_range_search("banana", "date", table);
    assert(sub != NULL);
    assert(sub->size == 2); // banana и carrot
    assert(strcmp(sub->ks[0].key, "banana") == 0);
    assert(strcmp(sub->ks[1].key, "carrot") == 0);

    free_table(sub);
    free_table(table);
}
