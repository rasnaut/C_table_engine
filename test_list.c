#include <stdio.h>
#include <stdlib.h>
#include "list.h"

// Умный assert с пояснением
#define CHECK(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "❌ Test failed: %s (%s:%d)\n", msg, __FILE__, __LINE__); \
            exit(1); \
        } else { \
            printf("✅ %s\n", msg); \
        } \
    } while (0)

void test_insert() {
    Node* list = NULL;

    for (int i = 0; i < 5; ++i) {
        int value = i * 10;
        list = node_insert(list, value);
        CHECK(list != NULL, "Insert: list non-null after insertion");
    }

    Node* n = node_find(list, 3);
    CHECK(n != NULL, "Insert: node with release 3 exists");
    CHECK(n->info == 30, "Insert: node with release 3 has correct value");

    node_destroy(list);
}

void test_find() {
    Node* list = NULL;

    int val = 42;
    list = node_insert(list, val);

    Node* found = node_find(list, 1);
    CHECK(found != NULL, "Find: found node with release 1");
    CHECK(found->info == 42, "Find: correct value found");

    Node* not_found = node_find(list, 99);
    CHECK(not_found == NULL, "Find: non-existent release not found");

    node_destroy(list);
}

void test_delete() {
    Node* list = NULL;

    for (int i = 0; i < 3; ++i) {
        list = node_insert(list, i + 1);
    }

    CHECK(node_delete(&list, 2) == 0, "Delete: removed release 2");
    CHECK(node_find(list, 2) == NULL, "Delete: release 2 no longer found");

    CHECK(node_delete(&list, 99) == -1, "Delete: removing non-existent release fails");

    node_destroy(list);
}

void test_destroy() {
    Node* list = NULL;

    for (int i = 0; i < 10; ++i) {
        list = node_insert(list, i);
    }

    node_destroy(list);
    CHECK(1, "Destroy: completed without crash");
}

int main() {
    printf("🔍 Running tests for linked list Node...\n\n");
    test_insert();
    test_find();
    test_delete();
    test_destroy();
    printf("\n🎉 All tests passed.\n");
    return 0;
}
