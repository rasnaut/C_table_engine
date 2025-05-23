#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "library.h"
#include "work_library.h"

void test_init_and_delete() {
    printf("🧪 Testing init and delete...\n");

    Table* table = malloc(sizeof(Table));
    assert(table != NULL);
    assert(core_init_table(table, 10) != NULL);
    assert(table->size == 0);
    assert(table->max_size == 10);

    free_table(table);
    printf("✅ Test init and delete passed!\n\n");
    
}

void test_init_and_insert() {
    printf("🧪 Testing insert and search...\n");

    Table* table = malloc(sizeof(Table));
    assert(table != NULL);
    assert(core_init_table(table, 10) != NULL);
    RelType release = core_insert("alpha", 42, table);
    assert(release > 0);
    assert(table->size == 1);
    printf("Insert one element ... Done!\n");

    KeySpace* element = core_search("alpha", table);
    assert(element != NULL);
    assert(element->busy == 1);
    assert(strcmp(element->key, "alpha") == 0);
    assert(element->list_length == 1);
    printf("Search 1 element ... Done!\n");
    assert(element->node->info == 42);
    printf("Founding 1 element data ... Done!\n");

    printf("Release 1: %d\n", release);
    release = core_insert("alpha", 99, table);
    printf("Release 2: %d\n", release);
    assert(release > 0); // добавляется как новая версия
    assert(element->list_length == 2);
    Node* found = node_find(element->node, release);
    assert(found != NULL);
    printf("Found info: %d\n", found->info);
    assert(found->info == 99);
    printf("Insert 2 element ... Done!\n");

    assert(core_insert("beta", 84, table) != -1);
    printf("Insert 3 element ... Done!\n");

    element = core_search("beta", table);
    assert(element != NULL);
    assert(element->busy == 1);
    assert(strcmp(element->key, "beta") == 0);
    assert(element->list_length == 1);
    assert(element->node != NULL);
    assert(element->node->info == 84);
    printf("Search 2 element ... Done!\n");

    printf("✅ Test insert and search passed!\n\n");
    free_table(table);
}

void test_search_and_delete() {
    printf("🧪 Testing search and delete by version...\n");

    Table* table = malloc(sizeof(Table));
    assert(core_init_table(table, 5) != NULL);

    RelType release1 = core_insert("x", 11, table);
    RelType release2 = core_insert("x", 22, table);
    RelType release3 = core_insert("x", 33, table);

    KeySpace* element = core_search("x", table);
    assert(element->busy == 1);
    assert(element->list_length == 3);
    

    // Удаляем версию 2
    assert(core_delete_by_key_and_release("x", release2, table) == 0);

    KeySpace* updated = core_search("x", table);
    assert(updated != NULL);
    assert(updated->list_length == 2); // нет удалённой версии
    assert(node_find(updated->node, release2) == NULL);

    // Удалим все оставшиеся
    core_delete_by_key_and_release("x", release1, table);
    core_delete_by_key_and_release("x", release3, table);
    assert(core_search("x", table) == NULL);

    printf("✅ Test search and delete passed!\n\n");
    free_table(table);
}

void test_search_not_found() {
    printf("🧪 Testing search not found...\n");

    Table* table = malloc(sizeof(Table));
    assert(core_init_table(table, 5) != NULL);

    assert(core_insert("one", 1, table) == 1);
    assert(core_insert("two", 2, table) == 1);

    assert(core_search("three", table) == NULL);
    assert(core_delete_by_key_and_release("two", 9, table) == -1); // версии 9 нет

    printf("✅ Test search not found passed!\n\n");
    free_table(table);
}

int main() {
    setbuf(stdout, NULL);  // отключает буфер stdout

    test_init_and_delete();
    test_init_and_insert();
    test_search_and_delete();
    test_search_not_found();

    printf("🎉 ✅ ALL TESTS PASSED!\n");
    return 0;
}
