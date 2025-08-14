#ifndef DIALOGUE_GRAPH_H
#define DIALOGUE_GRAPH_H

#include "graph/lib_graph.h"

// 1) создание/удаление
Graph* dialogue_create_graph(void);

// 2) добавить вершину
int dialogue_add_vertex(Graph* g);

// 3) добавить/изменить ребро (upsert: если есть — обновит вес)
int dialogue_add_or_update_edge(Graph* g);

// 4) изменить данные вершины (переименование)
int dialogue_edit_vertex(Graph* g);

// 5) изменить данные ребра (смена веса по именам вершин)
int dialogue_edit_edge(Graph* g);

// 6) удалить вершину
int dialogue_delete_vertex(Graph* g);

// 7) удалить ребро
int dialogue_delete_edge(Graph* g);

// 8) вывод графа списками смежности
void dialogue_print_adjacency(const Graph* g);

// 9) обход в ширину: знакомые не далее K рукопожатий
int dialogue_bfs_within_k(Graph* g);

// 10) кратчайшая положительная цепочка (Дейкстра)
int dialogue_shortest_positive_path(Graph* g);

// 11) разбиение на компоненты (положительные связи) и печать групп
int dialogue_print_positive_components(Graph* g);

// 12) визуализация graphviz
int dialogue_generate_graphviz(const Graph* g);

// 13) импорт графа из файла
// формат:
//   V <name>
//   E <from> <to> <weight>
// строки с # считаются комментариями
int dialogue_import_graph(Graph* g);

#endif // DIALOGUE_GRAPH_H
