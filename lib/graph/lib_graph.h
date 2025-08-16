#ifndef LIB_GRAPH_H
#define LIB_GRAPH_H

#include <stddef.h>
#include "lib_vertex.h"

// ----------------- Graph -----------------
typedef struct VertexNode {
    Vertex*            v;     // владеем
    struct VertexNode* next;
    struct VertexNode* prev;
} VertexNode;

typedef struct Graph {
    VertexNode* head;  // NULL => пусто; head->prev — хвост (кольцо)
    size_t      size;
} Graph;

Graph* graph_create(void);
void   graph_dispose(Graph* g);

// возвращает существующую при совпадении имени
Vertex* graph_add_vertex(Graph* g, const char* name_of_person);
int     graph_remove_vertex(Graph* g, const char* name_of_person);
Vertex* graph_find_vertex(Graph* g, const char* name_of_person);

size_t  graph_size(const Graph* g);
int     graph_is_empty(const Graph* g);

// 1) BFS до K рукопожатий; positive_only=1 — только по ребрам с weight>0.
// Возвращает 0 при успехе; out_names/ out_count — владение на вызывающей стороне (free каждого имени и массива).
int graph_bfs_within_k(
    Graph* g,
    const char* start_name,
    size_t max_hops,
    int positive_only,
    char*** out_names,
    size_t* out_count
);

// 2) Кратчайшая позитивная цепочка (только weight>0). cost_mode: 0 — стоимость=1; 1 — стоимость=11-weight.
int graph_shortest_positive_chain(
    Graph* g,
    const char* src_name,
    const char* dst_name,
    int cost_mode,
    char*** out_path_names,
    size_t* out_path_len,
    int* out_total_cost
);

// 3) Компоненты положительной достижимости
typedef struct GraphComponents {
    size_t   num_components;
    size_t*  component_sizes;  // len=num_components
    char***  names;            // names[i][j]
} GraphComponents;

// directed_mode: 0 — слабосвязные на неориентированном положительном подграфе; 1 — SCC на ориентированном положительном подграфе
int graph_positive_components(
    Graph* graph,
    int directed_mode,
    GraphComponents** out_result
);

#endif // LIB_GRAPH_H
