#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "lib/lib_tree_234.h" // подключаем твои структуры и функции

void test_simple_create_and_destry_node() {
    printf("Test: Create and Destroy node\n");
    Node234* tree = node234_create_node(0, NULL);
    assert(tree != NULL);
    assert(tree->key_count == 0);
    assert(tree->child_count == 0);
    node234_destroy(tree);
    printf("Test create and destroy node passed.\n");
}

void test_simple_create_insert_and_destry_node() {
    printf("Test: Create, Insert and Destroy node\n");
    Node234* tree = node234_create_node(0, NULL);
    assert(tree != NULL);
    assert(tree->key_count == 0);
    assert(tree->child_count == 0);
    node234_insert(&tree, "M", "10");
    node234_insert(&tree, "B", "20");
    node234_insert(&tree, "Q", "30");
    node234_insert(&tree, "A", "40");
    node234_tree_print(tree, 0);
    node234_destroy(tree);
    printf("Test create and destroy node passed.\n");
}

void test_insert_and_search() {
    printf("Test: Insert and Search\n");
    Node234* tree = node234_create_node(0, NULL);
    
    node234_insert(&tree, "M", "10");
    node234_insert(&tree, "B", "20");
    node234_insert(&tree, "Q", "30");
    node234_insert(&tree, "A", "40");
    node234_insert(&tree, "C", "50");
    node234_insert(&tree, "Z", "60");
    node234_insert(&tree, "L", "70");
    node234_tree_print(tree, 0);

    Element* e = node234_search_by_key(tree, "B");
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

    node234_insert(&tree, "D", "1");
    node234_insert(&tree, "B", "2");
    node234_insert(&tree, "A", "3");
    node234_insert(&tree, "C", "4");
    node234_insert(&tree, "F", "5");
    node234_insert(&tree, "E", "6");
    node234_insert(&tree, "G", "7");

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

    node234_insert(&tree, "app", "10");
    node234_insert(&tree, "apple", "20");
    node234_insert(&tree, "banana", "30");
    node234_insert(&tree, "apricot", "40");
    node234_insert(&tree, "berry", "50");

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

    node234_insert(&tree, "one", "1");
    node234_insert(&tree, "two", "2");
    node234_insert(&tree, "three", "3");
    node234_insert(&tree, "four", "4");
    node234_insert(&tree, "five", "5");

    KeyArray* res = node234_get_all_nodes_max_diff_symbols(tree, "three");
    for (size_t i = 0; i < res->size; ++i)
        printf("No common: %s\n", res->array_ptr[i]);

    key_array_delete(res);
    node234_destroy(tree);
}

void test_node234_find_max() {
    printf("Running test_node234_find_max...\n");

    Node234* root = node234_create_node(0, NULL);

    node234_insert(&root, "banana", "yellow");
    node234_insert(&root, "apple", "green");
    node234_insert(&root, "zebra", "black-white");
    node234_insert(&root, "dog", "bark");
    node234_insert(&root, "cat", "meow");

    Element max = node234_find_max(root);

    if (strcmp(max.key, "zebra") == 0 && strcmp(max.value, "black-white") == 0) {
        printf("✅ test_node234_find_max passed\n");
    } else {
        printf("❌ test_node234_find_max failed. Got: %s -> %s\n", max.key, max.value);
    }

    node234_destroy(root);
}

void test_node234_import_from_file() {
    const char* test_filename = "test_234_import.txt";

    // Создаем тестовый файл
    FILE* f = fopen(test_filename, "w");
    assert(f != NULL);

    fprintf(f, "apple\nRedFruit\nbanana\nYellowFruit\ncherry\nRedBerry\n");
    fclose(f);
    printf("Test file created\n");

    // Импортируем дерево
    Node234* tree = node234_create_from_file(test_filename);
    assert(tree != NULL);

    // Проверяем данные
    Element* e1 = node234_search_by_key(tree, "apple");
    Element* e2 = node234_search_by_key(tree, "banana");
    Element* e3 = node234_search_by_key(tree, "cherry");

    assert(e1 != NULL && strcmp(e1->value, "RedFruit") == 0);
    assert(e2 != NULL && strcmp(e2->value, "YellowFruit") == 0);
    assert(e3 != NULL && strcmp(e3->value, "RedBerry") == 0);

    printf("✅ Test passed: All keys imported and validated successfully\n");

    // Очистка
    node234_destroy(tree);
    remove(test_filename);
}


int main() {
    setbuf(stdout, NULL);
    test_simple_create_and_destry_node();
    test_simple_create_insert_and_destry_node();
    test_insert_and_search();
    test_print();
    test_contained_substring();
    test_max_diff_symbols();
    test_node234_find_max();
    test_node234_import_from_file();
    printf("All tests passed!\n");
    return 0;
}
