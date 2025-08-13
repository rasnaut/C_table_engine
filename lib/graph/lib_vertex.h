#ifndef LIB_VERTEX_H
#define LIB_VERTEX_H
#include "lib_vertex_handle.h"
#include "lib_edge_sorted_cycle_list.h"

// ----- Vertex -----
typedef struct Vertex {
    char*              name_of_person; // владеем
    EdgeSortedCycleList edges;         // список исходящих рёбер
    VertexHandle*      handle;         // владеем (обычно один на вершину)
} Vertex;

Vertex* vertex_create(const char* name_of_person); // создаёт и handle
void    vertex_dispose(Vertex* vertex);            // удаляет все рёбра, освобождает handle и имя
int     vertex_print(const Vertex* vertex);

// Добавление/удаление ребра (создаёт/освобождает Edge внутри)
int vertex_add_edge(Vertex* from, VertexHandle* to, int weight); // проверяет диапазон веса, отсутствие дубля (same 'to')
int vertex_remove_edge(Vertex* from, const VertexHandle* to);
int vertex_remove_edge_by_name(Vertex* from, const char* name_of_person);
int vertex_remove_invalid_edges(Vertex* v, size_t* removed_count); // Удаляет все исходящие рёбра на удалённые вершины (invalid handles).

#endif // LIB_VERTEX_H
