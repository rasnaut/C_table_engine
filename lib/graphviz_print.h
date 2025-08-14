#ifndef GRAPHVIZ_PRINT_H
#define GRAPHVIZ_PRINT_H

#include "graph/lib_graph.h"

// Генерирует .dot-файл для текущего состояния графа.
// Пример использования:
//   graph_generate_dot(g, "people.dot");
// Затем: dot -Tpng people.dot -o people.png
void graph_generate_dot(const Graph* graph, const char* filename);

#endif // GRAPHVIZ_PRINT_H