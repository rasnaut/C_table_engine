// graphviz_print.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graphviz_print.h"
#include "graph/lib_edge.h"
#include "graph/lib_vertex_handle.h"
#include "graph/lib_edge_sorted_cycle_list.h"

// безопасно обрезаем строку для метки (Graphviz label)
static void trim_label(const char* src, char* dst, size_t dst_cap) {
    if (!dst || dst_cap == 0) return;
    if (!src) { dst[0] = '\0'; return; }
    // ограничим длину, но без экранирования спецсимволов — если нужно, можно добавить
    snprintf(dst, dst_cap, "%.60s", src);
}

void graph_generate_dot(const Graph* graph, const char* filename) {
    if (!graph || !filename) return;

    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s for writing\n", filename);
        return;
    }

    fprintf(file, "digraph PeopleGraph {\n");
    fprintf(file, "    rankdir=LR;\n");
    fprintf(file, "    node [shape=ellipse, style=filled, fillcolor=lightyellow, fontname=\"Helvetica\"];\n");
    fprintf(file, "    edge [fontname=\"Helvetica\"];\n\n");

    // 1) узлы (вершины)
    if (graph->head && graph->size > 0) {
        const VertexNode* current = graph->head;
        for (size_t i = 0; i < graph->size; ++i) {
            if (current->v) {
                char name_buf[64];
                trim_label(current->v->name_of_person, name_buf, sizeof(name_buf));
                // Узел идентифицируем адресом Vertex*
                fprintf(file, "    \"v_%p\" [label=\"%s\"];\n", (void*)current->v, name_buf);
            }
            current = current->next;
        }
    }
    fprintf(file, "\n");

    // 2) рёбра
    if (graph->head && graph->size > 0) {
        const VertexNode* from_node = graph->head;
        for (size_t i = 0; i < graph->size; ++i) {
            const Vertex* from_v = from_node->v;
            if (from_v && from_v->edges.head && from_v->edges.size > 0) {
                const Edge* edge_iter = from_v->edges.head;
                for (size_t k = 0; k < from_v->edges.size; ++k) {
                    const VertexHandle* vertexHandle = edge_iter->to;
                    int weight = edge_iter->weight;

                    // Определим оформление ребра
                    const char* color = "gray50";
                    const char* style = "solid";
                    if (vertexHandle && vertexHandle->vertex) {
                        if      (weight > 0) color = "forestgreen";
                        else if (weight < 0) color = "firebrick";
                        else                  color = "gray50";
                    } else {
                        // невалидная цель — пунктиром и серым
                        color = "gray60";
                        style = "dashed";
                    }

                    // Подпись веса
                    char label_buf[32];
                    snprintf(label_buf, sizeof(label_buf), "%d", weight);

                    // Откуда
                    fprintf(file, "    \"v_%p\" -> ", (void*)from_v);

                    // Куда
                    if (vertexHandle && vertexHandle->vertex && vertexHandle->vertex->name_of_person) {
                        // валидная цель
                        fprintf(file, "\"v_%p\" ", (void*)vertexHandle->vertex);
                    } else {
                        // невалидная цель: рисуем отдельный узел с меткой "(invalid)"
                        fprintf(file, "\"inv_%p\" ", (void*)vertexHandle); // связываем с адресом handle
                        fprintf(file, "[color=%s, style=%s, label=\"%s\"];\n",
                                color, style, label_buf);
                        // нарисуем сам узел недоступной цели (разово — но если много, ничего страшного)
                        fprintf(file, "    \"inv_%p\" [shape=box, style=dashed, color=gray60, "
                                      "fontcolor=gray30, label=\"(invalid)\"];\n",
                                (void*)vertexHandle);
                        // следующая итерация
                        edge_iter = edge_iter->next;
                        continue;
                    }

                    // Атрибуты ребра (для валидной цели)
                    fprintf(file, "[color=%s, style=%s, label=\"%s\"];\n",
                            color, style, label_buf);

                    edge_iter = edge_iter->next;
                }
            }
            from_node = from_node->next;
        }
    }

    fprintf(file, "}\n");
    fclose(file);
    printf("Graphviz dot-файл сгенерирован: %s\n", filename);
}
