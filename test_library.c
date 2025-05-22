#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "library.h"
#include "work_library.h"


void test_init_and_free() {
    printf("Testing init and free...\n");
    Table* table = malloc(sizeof(Table));
    assert(table != NULL);

    assert(core_init_table(table, 10) != NULL);
    assert(table->size == 0);
    assert(table->max_size == 10);
    
    free_table(table);
    printf("Test init and insert passed!\n");
}

void test_init_and_insert() {
    printf("Testing init and insert...\n");
    Table* table = malloc(sizeof(Table));
    assert(table != NULL);

    assert(core_init_table(table, 10) != NULL);
    assert(table->size == 0);
    assert(table->max_size == 10);
    assert(core_insert("alpha\0", 42, table) == 0);
    assert(table->size == 1);
    assert(table->ks != NULL);
    
    assert(strcmp(table->ks[0].key, "alpha") == 0);
    
    printf("middle part done\n");
    

    assert(core_insert("beta", 84, table) == 0);
    assert(table->size == 2);
    assert(strcmp(table->ks[1].key, "beta") == 0);
    assert(table->ks[1].info == 84);

    // вставка дубликата
    assert(core_insert("alpha", 99, table) == -1);

    free_partial_table(table, table->size);
    assert(table->ks != NULL);
    for(int index = 0; index < table->size; index++) {
        assert(table->ks[index].key == NULL);
    }
    printf("All indx cleaned\n");
    free_table(table);
    printf("Test init and insert passed!\n");
}

void test_search_and_delete() {
    printf("Testing search and delete...\n");
    Table* table = malloc(sizeof(Table));
    assert(core_init_table(table, 5) != NULL);

    core_insert("a", 1, table);
    core_insert("b", 2, table);
    core_insert("c", 3, table);

    int idx = core_binary_search("b", table);
    assert(idx != -1);
    printf("core_binary_search execute 1 ... passed\n");
    assert(strcmp(table->ks[idx].key, "b") == 0);
    printf("core_binary_search check 1 ... passed\n");

    // Удалим и проверим, что его больше нет
    assert(core_delete("b", table) == 0);
    printf("core_delete 1 ... passed\n");
    assert(core_binary_search("b", table) == -1);
    printf("core_binary_search execute 2 ... passed\n");
    assert(table->size == 2);

    // Удаление несуществующего
    assert(core_delete("zzz", table) == -1);
    printf("core_delete 2 ... passed\n");

    free_table(table);
    printf("Test search and delete passed!\n");
}

void test_range_search() {
    printf("Testing range search...\n");
    Table* table = malloc(sizeof(Table));
    core_init_table(table, 10);

    core_insert("apple", 1, table);
    core_insert("banana", 2, table);
    core_insert("carrot", 3, table);
    core_insert("date", 4, table);
    core_insert("fig", 5, table);

    core_print_table(table);

    Table* sub = core_range_search("banana", "date", table);
    core_print_table(sub);
    assert(sub != NULL);
    printf("sub->size == %d\n", sub->size);
    assert(sub->size == 2); // banana и carrot
    assert(strcmp(sub->ks[0].key, "banana") == 0);
    assert(strcmp(sub->ks[1].key, "carrot") == 0);
    
    free_table(sub);
    free_table(table);
    printf("Test range search passed!\n");
}

int main() {
    setbuf(stdout, NULL);  // отключает буфер stdout
    test_init_and_insert();
    test_search_and_delete();
    test_range_search();
    printf("✅ All tests passed!\n");
    return 0;
}