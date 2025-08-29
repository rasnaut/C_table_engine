#ifndef LIB_EDGE_H
#define LIB_EDGE_H

#include "lib_vertex_handle.h"

typedef struct Edge {
    VertexHandle* to;   // владеем +1 ссылкой
    int           weight; // [-10..+10]
    struct Edge*  next; // для кольцевого отсортированного списка
    struct Edge*  prev;
} Edge;

int  edge_init(Edge* edge, VertexHandle* to, int weight); // берет add_ref(to)
void edge_dispose(Edge* edge); // делает release(to), не трогает связность списка
int  edge_print(const Edge* edge);
// Сравнение для сортированной вставки: сперва по имени вершины назначения, затем по весу
int  edge_compare(const Edge* a, const Edge* b);
int edge_check_weight_more_then(const Edge* edge, int threshold);

#endif // LIB_EDGE_H