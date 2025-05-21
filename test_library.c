#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "library.h"
#include "work_library.h"

void test_init_and_insert() {
    printf("🧪 Testing init and insert...\n");

    Table* table = malloc(sizeof(Table));
    assert(table != NULL);
    assert(core_init_table(table, 10) != NULL);
    assert(table->size == 0);
    assert(table->max_size == 10);

    assert(core_insert("alpha", 42, table) == 0);
    assert(table->size == 1);
    Node* n = core_search("alpha", table);
    assert(n != NULL);
    assert(*(unsigned int*)n->info == 42);

    assert(core_insert("alpha", 99, table) == 0); // добавляется как новая версия
    assert(n->next != NULL);
    assert(*(unsigned int*)n->next->info == 99);

    assert(core_insert("beta", 84, table) == 0);
    assert(core_search("beta", table) != NULL);

    printf("✅ Test init and insert passed!\n\n");
    free_table(table);
}

void test_search_and_delete() {
    printf("🧪 Testing search and delete by version...\n");

    Table* table = malloc(sizeof(Table));
    assert(core_init_table(table, 5) != NULL);

    core_insert("x", 11, table);
    core_insert("x", 22, table);
    core_insert("x", 33, table);

    Node* chain = core_search("x", table);
    assert(chain && chain->next && chain->next->next);
    assert(*(unsigned int*)chain->info == 11);
    assert(*(unsigned int*)chain->next->info == 22);
    assert(*(unsigned int*)chain->next->next->info == 33);

    // Удаляем версию 1
    assert(core_delete_by_key_and_release("x", 1, table) == 0);

    Node* updated = core_search("x", table);
    assert(updated != NULL);
    assert(updated->release != 1); // нет удалённой версии

    // Удалим все оставшиеся
    core_delete_by_key_and_release("x", 0, table);
    core_delete_by_key_and_release("x", 2, table);
    assert(core_search("x", table) == NULL);

    printf("✅ Test search and delete passed!\n\n");
    free_table(table);
}

void test_search_not_found() {
    printf("🧪 Testing search not found...\n");

    Table* table = malloc(sizeof(Table));
    assert(core_init_table(table, 5) != NULL);

    assert(core_insert("one", 1, table) == 0);
    assert(core_insert("two", 2, table) == 0);

    assert(core_search("three", table) == NULL);
    assert(core_delete_by_key_and_release("two", 9, table) == -1); // версии 9 нет

    printf("✅ Test search not found passed!\n\n");
    free_table(table);
}

int main() {
    setbuf(stdout, NULL);  // отключает буфер stdout

    test_init_and_insert();
    test_search_and_delete();
    test_search_not_found();

    printf("🎉 ✅ ALL TESTS PASSED!\n");
    return 0;
}
