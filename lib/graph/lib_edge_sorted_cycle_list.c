#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lib_edge_sorted_cycle_list.h"
#include "lib_vertex.h"

// Вспомогательная: поиск дубликата по handle
static Edge* find_by_handle_internal(const EdgeSortedCycleList* list, const VertexHandle* to) {
    if (!list || !list->head || !to) return NULL;
    Edge* cur = list->head;
    for (size_t i = 0; i < list->size; i++) {
        if (cur->to == to) return cur; // сравниваем по адресу handle
        cur = cur->next;
    }
    return NULL;
}

// helper: проверка принадлежности node списку (по адресу)
static int list_contains_node(const EdgeSortedCycleList* list, const Edge* node) {
    if (!list || !list->head || !node) return 0;
    const Edge* cur = list->head;
    for (size_t i = 0; i < list->size; ++i) {
        if (cur == node) return 1;
        cur = cur->next;
    }
    return 0;
}

void edge_list_init(EdgeSortedCycleList* list) {
    if (!list) return;
    list->head = NULL;
    list->size = 0;
}

void edge_list_dispose(EdgeSortedCycleList* list) {
    if (!list || !list->head) return;
    Edge* cur = list->head;
    size_t count = list->size;
    for (size_t i = 0; i < count; i++) {
        Edge* next = cur->next;
        edge_dispose(cur);
        free(cur);
        cur = next;
    }
    list->head = NULL;
    list->size = 0;
}

int edge_list_upsert(EdgeSortedCycleList* list, Edge* node, int* is_update) {
    if (is_update) *is_update = 0;
    if (!list || !node || !node->to) return -1;

    // Ищем дубликат по VertexHandle*
    Edge* dup = edge_list_find_by_handle(list, node->to);
    if (dup) {
        // обновление веса
        dup->weight = node->weight;
        // освобождаем ПЕРЕДАННЫЙ узел, т.к. он не понадобился
        edge_dispose(node);
        free(node);
        if (is_update) *is_update = 1;
        return 0;
    }

    // обычная сортированная вставка (как раньше в insert)
    if (!list->head) {
        node->next = node->prev = node;
        list->head = node;
        list->size = 1;
        return 0;
    }

    Edge* cur = list->head;
    for (size_t i = 0; i < list->size; ++i) {
        if (edge_compare(node, cur) < 0) {
            Edge* prev = cur->prev;
            node->next = cur;
            node->prev = prev;
            prev->next = node;
            cur->prev = node;
            if (cur == list->head) list->head = node;
            list->size++;
            return 0;
        }
        cur = cur->next;
    }
    // в конец (перед head)
    Edge* tail = list->head->prev;
    node->next = list->head;
    node->prev = tail;
    tail->next = node;
    list->head->prev = node;
    list->size++;
    return 0;
}

int edge_list_remove(EdgeSortedCycleList* list, Edge* node) {
    if (!list || !list->head || !node) return -1;
    if (!list_contains_node(list, node)) return -1;

    if (list->size == 1) {
        edge_dispose(node);
        free(node);
        list->head = NULL;
        list->size = 0;
        return 0;
    }
    // отсоединяем
    node->prev->next = node->next;
    node->next->prev = node->prev;
    if (list->head == node) list->head = node->next;
    list->size--;
    edge_dispose(node);
    free(node);
    return 0;
}

Edge* edge_list_find_by_handle(const EdgeSortedCycleList* list, const VertexHandle* to) {
    return find_by_handle_internal(list, to);
}

int edge_list_remove_by_handle(EdgeSortedCycleList* list, const VertexHandle* to) {
    Edge* e = find_by_handle_internal(list, to);
    if (!e) return -1;
    return edge_list_remove(list, e);
}

int edge_list_remove_invalid(EdgeSortedCycleList* list, size_t* removed_count) {
    if (removed_count) *removed_count = 0;
    if (!list || !list->head || list->size == 0) return 0;

    size_t n = list->size;
    Edge* cur = list->head;
    for (size_t i = 0; i < n; ++i) {
        Edge* next = cur->next;
        if (vertex_handle_is_null(cur->to)) {
             if (edge_list_remove(list, cur) == 0 && removed_count) {
                (*removed_count)++;
            }
        }
        cur = (list->head ? next : NULL);
        if (!cur) break;
    }
    return 0;
}


// --- find by name ---
Edge* edge_list_find_by_name(const EdgeSortedCycleList* list, const char* name) {
    if (!list || !list->head || !name) return NULL;
    Edge* cur = list->head;
    for (size_t i = 0; i < list->size; ++i) {
        const char* nm = NULL;
        if (cur->to && cur->to->vertex) nm = cur->to->vertex->name_of_person;
        if (nm && strcmp(nm, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

int edge_list_is_empty(const EdgeSortedCycleList* list) {
    return !list || list->head == NULL;
}

size_t edge_list_size(const EdgeSortedCycleList* list) {
    return list ? list->size : 0;
}

void edge_list_print(const EdgeSortedCycleList* list) {
    if (!list || !list->head) {
        printf("(empty)\n");
        return;
    }
    Edge* cur = list->head;
    for (size_t i = 0; i < list->size; i++) {
        edge_print(cur);
        cur = cur->next;
    }
}
