#include <string.h>
#include <stdio.h>
#include "lib_edge.h"
#include "lib_vertex_handle.h"
#include "lib_vertex.h"

int edge_init(Edge *edge, VertexHandle *to, int weight)
{
    if (!edge || !to || weight < -10 || weight > 10) {
        fprintf(stderr, "Error: Invalid arguments for edge initialization\n");
        return -1;
    }
    
    edge->weight = weight;
    edge->next = NULL;
    edge->prev = NULL;
    if (vertex_handle_add_ref(to) != 0) {
        fprintf(stderr, "Error: Failed to add reference to VertexHandle\n");
        return -1;
    }
    edge->to = to;
    return 0;
}

void edge_dispose(Edge* edge){
    if (!edge) return;

    // Освобождаем ссылку на вершину назначения
    if (edge->to) {
        vertex_handle_release(edge->to);
    }
}

int  edge_print(const Edge* edge) {
    const char* name = "(invalid)";
    int weight = 0;
    if (edge && !vertex_handle_is_null(edge->to) && edge->to->vertex->name_of_person) {
        name = edge->to->vertex->name_of_person;
        weight = edge->weight; 
    }
    printf("Edge to: %s, Weight: %d\n", name, weight);
    return 0;
}

// Сравнение для сортированной вставки: сперва по имени вершины назначения, затем по весу
int  edge_compare(const Edge* a, const Edge* b) {
    int a_valid = a && a->to && a->to->vertex && a->to->vertex->name_of_person;
    int b_valid = b && b->to && b->to->vertex && b->to->vertex->name_of_person;

    if (!a_valid && !b_valid) return 0;  // оба невалидны → равны
    if (!a_valid) return 1;              // a позже
    if (!b_valid) return -1;             // a раньше
    
    int cmp = strcmp(a->to->vertex->name_of_person, b->to->vertex->name_of_person);
    if (cmp == 0) {
        return a->weight - b->weight; // Compare by weight if names are equal
    }
    return cmp; // Compare by name
}

int edge_check_weight_more_then(const Edge *edge, int threshold)
{
    if (!edge) return 0;
    if (edge->to && edge->to->vertex) {
        return edge->weight > threshold;
    }
    return 0;
}
