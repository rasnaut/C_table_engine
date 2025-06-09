#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib_binary_tree.h"
#include "graphviz_print.h"

void test_create_and_destory() {
    BinaryTree* tree = binary_tree_create();
    assert(tree != NULL);

    binary_tree_destroy(tree);
    printf("✅ test_create_and_destory passed\n");
}

void test_create_insert_and_destory() {
    BinaryTree* tree = binary_tree_create();
    assert(tree != NULL);

    binary_tree_insert(tree, "k1", "data1");
    binary_tree_insert(tree, "k2", "data2");
    binary_tree_insert(tree, "k3", "data3");

    binary_tree_destroy(tree);
    printf("✅ test_create_insert_and_destory passed\n");
}

void test_insert_and_search() {
    printf("test_insert_and_search START\n");
    BinaryTree* tree = binary_tree_create();
    assert(tree != NULL);

    binary_tree_insert(tree, "k1", "data1");
    binary_tree_insert(tree, "k2", "data2");
    binary_tree_insert(tree, "k3", "data3");

    BinaryTreeNode* n1 = binary_tree_search(tree, "k1");
    BinaryTreeNode* n2 = binary_tree_search(tree, "k2");
    BinaryTreeNode* n3 = binary_tree_search(tree, "k3");

    assert(n1 != NULL && strcmp(n1->data, "data1") == 0);
    assert(n2 != NULL && strcmp(n2->data, "data2") == 0);
    assert(n3 != NULL && strcmp(n3->data, "data3") == 0);

    BinaryTreeNode* missing = binary_tree_search(tree, "missing");
    assert(missing == NULL);

    binary_tree_destroy(tree);
    printf("✅ test_insert_and_search passed\n");
}

void test_duplicate_insertion() {
    BinaryTree* tree = binary_tree_create();
    binary_tree_insert(tree, "key", "first");
    binary_tree_insert(tree, "key", "second");  // should update

    BinaryTreeNode* node = binary_tree_search(tree, "key");
    assert(node != NULL);
    assert(strcmp(node->data, "second") == 0);

    binary_tree_destroy(tree);
    printf("✅ test_duplicate_insertion passed\n");
}

void test_filtering() {
    BinaryTree* tree = binary_tree_create();
    binary_tree_insert(tree, "apple", "apple");
    binary_tree_insert(tree, "banana", "banana");
    binary_tree_insert(tree, "carrot", "carrot");
    binary_tree_insert(tree, "canon", "ca");
    binary_tree_insert(tree, "ananas", "ca");

    binary_tree_print_sort_view(tree);
    BinaryTree* result = binary_tree_create();
    binary_tree_get_all_nodes_contained_substring(tree->root, "ca", result);
    binary_tree_print_sort_view(result);

    BinaryTreeNode* missing1 = binary_tree_search(result, "banana");
    assert(missing1 == NULL);

    BinaryTreeNode* missing = binary_tree_search(result, "apple");
    assert(missing == NULL);

    BinaryTreeNode* found = binary_tree_search(result, "carrot");
    assert(found != NULL);
    assert(strcmp(found->data, "carrot") == 0);

    found = binary_tree_search(result, "canon");
    assert(found != NULL);
    assert(strcmp(found->data, "ca") == 0);

    binary_tree_destroy(tree);
    binary_tree_destroy(result);
    printf("✅ test_filtering passed\n");
}

void test_special_search() {
    BinaryTree* tree = binary_tree_create();
    binary_tree_insert(tree, "apple", "apple");
    binary_tree_insert(tree, "banana", "banana");
    binary_tree_insert(tree, "carrot", "carrot");
    binary_tree_insert(tree, "canon", "ca");
    binary_tree_insert(tree, "ananas", "ca");

    binary_tree_print_sort_view(tree);
    
    KeyArray* keyArray =  binary_tree_get_all_nodes_max_diff_symbols(tree, "n");
    assert(keyArray != NULL);
    key_array_print(keyArray);
    //assert(keyArray->size == 3); // "banana", "carrot", "ananas"
    

    binary_tree_destroy(tree);
    key_array_delete(keyArray);
    printf("✅ test_special_search passed\n");
}

void test_erase_leaf_node() {
    BinaryTree* tree = binary_tree_create();
    binary_tree_insert(tree, "b", "middle");
    binary_tree_insert(tree, "a", "left");
    binary_tree_insert(tree, "c", "right"); // <-- будет листом

    assert(binary_tree_erase_node_by_key(tree, "c") == 0);

    assert(binary_tree_search(tree, "c") == NULL);
    assert(binary_tree_search(tree, "a") != NULL);
    assert(binary_tree_search(tree, "b") != NULL);

    binary_tree_destroy(tree);
    printf("test_erase_leaf_node passed\n");
}

void test_erase_node_with_one_child() {
    BinaryTree* tree = binary_tree_create();
    binary_tree_insert(tree, "a", "root");
    binary_tree_insert(tree, "b", "right child");

    assert(binary_tree_erase_node_by_key(tree, "a") == 0);

    assert(binary_tree_search(tree, "a") == NULL);
    assert(binary_tree_search(tree, "b") != NULL);
    assert(strcmp(binary_tree_search(tree, "b")->data, "right child") == 0);

    binary_tree_destroy(tree);
    printf("test_erase_node_with_one_child passed\n");
}

void test_erase_node_with_two_children() {
    BinaryTree *tree = binary_tree_create();
    binary_tree_insert(tree, "m", "root");
    binary_tree_insert(tree, "c", "left");
    binary_tree_insert(tree, "t", "right");
    binary_tree_insert(tree, "z", "right right");
    binary_tree_insert(tree, "s", "right left");

    binary_tree_print(tree);

    assert(binary_tree_erase_node_by_key(tree, "t") == 0);

    binary_tree_print(tree);

    assert(binary_tree_search(tree, "t") == NULL);
    assert(binary_tree_search(tree, "s") != NULL);
    assert(binary_tree_search(tree, "z") != NULL);
    assert(binary_tree_search(tree, "m") != NULL);

    binary_tree_destroy(tree);
    printf("test_erase_node_with_two_children passed\n");
}

void test_erase_root_node() {
    BinaryTree *tree = binary_tree_create();
    binary_tree_insert(tree, "x", "root");
    binary_tree_insert(tree, "a", "left");
    binary_tree_insert(tree, "z", "right");

    assert(binary_tree_erase_node_by_key(tree, "x") == 0);

    assert(binary_tree_search(tree, "x") == NULL);
    assert(binary_tree_search(tree, "a") != NULL);
    assert(binary_tree_search(tree, "z") != NULL);

    binary_tree_destroy(tree);
    printf("test_erase_root_node passed\n");
}

void test_erase() {
    test_erase_leaf_node();
    test_erase_node_with_one_child();
    test_erase_node_with_two_children();
    test_erase_root_node();
    printf("All binary tree erase tests passed!\n"); 
}


void test_binary_tree_create_from_file() {
    printf("START test_binary_tree_create_from_file\n"); 
    const char* filename = "test_tree_input.txt";

    // Шаг 1: записываем тестовый файл
    FILE* file = fopen(filename, "w");
    assert(file != NULL);
    fprintf(file, "alpha\nvalue1\nbeta\nvalue2\ngamma\nvalue3\n");
    fclose(file);
    printf("STEP 1 Succsesfully done! File created\n"); 

    // Шаг 2: создаём дерево из файла
    BinaryTree* tree = binary_tree_create_from_file(filename);
    assert(tree != NULL);
    assert(tree->size == 3);
    printf("STEP 2 Succsesfully done! Binary tree created\n"); 

    binary_tree_print(tree);

    // Шаг 3: проверяем наличие ключей
    BinaryTreeNode* e1 = binary_tree_search(tree, "alpha");
    BinaryTreeNode* e2 = binary_tree_search(tree, "beta");
    BinaryTreeNode* e3 = binary_tree_search(tree, "gamma");

    assert(e1 != NULL && strcmp(e1->data, "value1") == 0);
    assert(e2 != NULL && strcmp(e2->data, "value2") == 0);
    assert(e3 != NULL && strcmp(e3->data, "value3") == 0);
    printf("STEP 3 Succsesfully done! Binary tree worked well\n"); 

    // Шаг 4: чистим память
    binary_tree_destroy(tree);
    remove(filename);

    printf("test_binary_tree_create_from_file passed!\n");
}

void test_generate_dot() {
    printf("Running test_generate_dot...\n");
    BinaryTree* tree = binary_tree_create();
    assert(tree);

    binary_tree_insert(tree, "D", "4");
    binary_tree_insert(tree, "B", "2");
    binary_tree_insert(tree, "A", "1");
    binary_tree_insert(tree, "C", "3");
    binary_tree_insert(tree, "F", "6");
    binary_tree_insert(tree, "E", "5");

    binary_tree_generate_dot(tree, "tree_output.dot");

    FILE* f = fopen("tree_output.dot", "r");
    assert(f != NULL);
    fclose(f);

    printf("DOT файл успешно создан.\n");

    binary_tree_destroy(tree);
}


int main() {
    setbuf(stdout, NULL); // ensure printf shows immediately
    printf("START TESTS:\n");
    test_create_and_destory();
    test_create_insert_and_destory();
    test_insert_and_search();
    test_duplicate_insertion();
    test_filtering();
    test_erase();
    test_special_search();
    test_binary_tree_create_from_file();
    test_generate_dot();
    printf("\n🎉 All binary tree tests passed!\n");
    return 0;
}
