#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib_binary_tree.h"

void test_insert_and_search() {
    BinaryTree* tree = binary_tree_create();
    assert(tree != NULL);

    binary_tree_insert(tree, "k1", "data1");
    binary_tree_insert(tree, "k2", "data2");
    binary_tree_insert(tree, "k3", "data3");

    BinaryTreeNode* n1 = binary_tree_search(tree->root, "k1");
    BinaryTreeNode* n2 = binary_tree_search(tree->root, "k2");
    BinaryTreeNode* n3 = binary_tree_search(tree->root, "k3");

    assert(n1 != NULL && strcmp(n1->data, "data1") == 0);
    assert(n2 != NULL && strcmp(n2->data, "data2") == 0);
    assert(n3 != NULL && strcmp(n3->data, "data3") == 0);

    BinaryTreeNode* missing = binary_tree_search(tree->root, "missing");
    assert(missing == NULL);

    binary_tree_destroy(tree);
    printf("✅ test_insert_and_search passed\n");
}

void test_duplicate_insertion() {
    BinaryTree* tree = binary_tree_create();
    binary_tree_insert(tree, "key", "first");
    binary_tree_insert(tree, "key", "second");  // should update

    BinaryTreeNode* node = binary_tree_search(tree->root, "key");
    assert(node != NULL);
    assert(strcmp(node->data, "second") == 0);

    binary_tree_destroy(tree);
    printf("✅ test_duplicate_insertion passed\n");
}

void test_filtering() {
    BinaryTree* tree = binary_tree_create();
    binary_tree_insert(tree, "a", "apple");
    binary_tree_insert(tree, "b", "banana");
    binary_tree_insert(tree, "c", "carrot");

    BinaryTree* result = binary_tree_create();
    binary_tree_get_all_nodes_contained_substring(tree->root, "an", result);

    BinaryTreeNode* found = binary_tree_search(result->root, "b");
    assert(found != NULL);
    assert(strcmp(found->data, "banana") == 0);

    BinaryTreeNode* missing = binary_tree_search(result->root, "a");
    assert(missing == NULL);

    binary_tree_destroy(tree);
    binary_tree_destroy(result);
    printf("✅ test_filtering passed\n");
}

int main() {
    setbuf(stdout, NULL); // ensure printf shows immediately
    test_insert_and_search();
    test_duplicate_insertion();
    test_filtering();
    printf("\n🎉 All binary tree tests passed!\n");
    return 0;
}
