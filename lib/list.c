#include <stdlib.h>
#include <stdio.h>
#include "list.h"

Node* node_create(InfoType info, RelType release) {
    Node* node = malloc(sizeof(Node));
    if (!node) return NULL;
    node->release = release;
    node->info = info;
    node->next = NULL;
    return node;
}

Node* node_insert(Node* head, InfoType info) {
    RelType next_release = 1;
    Node* curr = head;
    while (curr && curr->next) {
        curr = curr->next;
        next_release++;
    }

    Node* new_node = node_create(info, next_release);
    if (!new_node) return NULL;

    if (!head)
        return new_node;

    curr->next = new_node;
    return head;
}

Node* node_find(Node* head, RelType release) {
    Node* curr = head;
    while (curr) {
        if (curr->release == release)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

int node_delete(Node** head, RelType release) {
    if (!head || !*head) return -1;

    Node* curr = *head;
    Node* prev = NULL;

    while (curr) {
        if (curr->release == release) {
            if (prev) prev->next = curr->next;
            else *head = curr->next;

            free(curr->info);
            free(curr);
            return 0;
        }
        prev = curr;
        curr = curr->next;
    }
    return -1;
}

void node_destroy(Node* head) {
    Node* curr = head;
    while (curr) {
        Node* next = curr->next;
        free(curr->info);
        free(curr);
        curr = next;
    }
}

void print_list(const Node* head) {
    const Node* curr = head;
    while (curr) {
        printf("  [release: %u] value: %u\n", curr->release, *(unsigned int*)curr->info);
        curr = curr->next;
    }
}
