#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "lib/lib_tree_234.h" // подключаем твои структуры и функции

void test_insert_and_search() {
    printf("Test: Insert and Search\n");
    Node234* tree = node234_create_node(0, NULL);

    insert(&tree, "M", "10");
    insert(&tree, "B", "20");
    insert(&tree, "Q", "30");
    insert(&tree, "A", "40");
    insert(&tree, "C", "50");
    insert(&tree, "Z", "60");
    insert(&tree, "L", "70");
    node234_tree_print(tree, 0);

    Element* e = node234_search_by_key(tree, "B");
    if(e == NULL) {
        printf("Element with key 'B' not found.\n");
    } else {
        printf("Found element with key 'B': %s\n", e->value);
    }
    assert(e != NULL && strcmp(e->value, "20") == 0);

    e = node234_search_by_key(tree, "Z");
    assert(e != NULL && strcmp(e->value, "60") == 0);

    e = node234_search_by_key(tree, "X");
    assert(e == NULL);

    printf("Test insert and search passed.\n");
    node234_destroy(tree);
}

void test_print() {
    printf("Test: Tree print\n");
    Node234* tree = node234_create_node(0, NULL);

    insert(&tree, "D", "1");
    insert(&tree, "B", "2");
    insert(&tree, "A", "3");
    insert(&tree, "C", "4");
    insert(&tree, "F", "5");
    insert(&tree, "E", "6");
    insert(&tree, "G", "7");

    printf("Tree structure:\n");
    node234_tree_print(tree, 0);

    printf("Sorted view: ");
    node234_print_sort_view(tree);
    printf("\n");

    node234_destroy(tree);
}

void test_contained_substring() {
    printf("Test: Get keys with prefix\n");
    Node234* tree = node234_create_node(0, NULL);

    insert(&tree, "app", "10");
    insert(&tree, "apple", "20");
    insert(&tree, "banana", "30");
    insert(&tree, "apricot", "40");
    insert(&tree, "berry", "50");

    KeyArray* res = node234_get_all_nodes_contained_substring(tree, "app");
    assert(res && res->size == 2);

    for (size_t i = 0; i < res->size; ++i)
        printf("Matched: %s\n", res->array_ptr[i]);

    key_array_delete(res);
    node234_destroy(tree);
}

void test_max_diff_symbols() {
    printf("Test: Get keys with no common symbols\n");
    Node234* tree = node234_create_node(0, NULL);

    insert(&tree, "one", "1");
    insert(&tree, "two", "2");
    insert(&tree, "three", "3");
    insert(&tree, "four", "4");
    insert(&tree, "five", "5");

    KeyArray* res = node234_get_all_nodes_max_diff_symbols(tree, "three");
    for (size_t i = 0; i < res->size; ++i)
        printf("No common: %s\n", res->array_ptr[i]);

    key_array_delete(res);
    node234_destroy(tree);
}

int main() {
    test_insert_and_search();
    test_print();
    test_contained_substring();
    test_max_diff_symbols();
    printf("All tests passed!\n");
    return 0;
}
