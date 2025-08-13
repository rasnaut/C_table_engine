#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "lib_graph.h"
#include "lib_edge.h"
#include "lib_edge_sorted_cycle_list.h"
#include "lib_vertex_handle.h"

// На всякий случай: прототип, если не добавлен в заголовок списка
int edge_list_remove_invalid(EdgeSortedCycleList* list, size_t* removed_count);

// =============================
// Вспомогательные структуры и функции уровня Graph

static VertexNode* vertex_node_create(Vertex* vertex) {
    VertexNode* node = (VertexNode*)malloc(sizeof(VertexNode));
    if (!node) return NULL;
    node->v   = vertex;
    node->next = node->prev = NULL;
    return node;
}

Graph* graph_create(void) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) return NULL;
    graph->head = NULL;
    graph->size = 0;
    return graph;
}

static void graph_unlink_node(Graph* graph, VertexNode* node) {
    if (!graph || !node || !graph->head) return;
    if (graph->size == 1 && graph->head == node) {
        graph->head = NULL;
        graph->size = 0;
        return;
    }
    node->prev->next = node->next;
    node->next->prev = node->prev;
    if (graph->head == node) graph->head = node->next;
    graph->size--;
}

void graph_dispose(Graph* graph) {
    if (!graph) return;
    if (graph->head) {
        size_t total_nodes = graph->size;
        VertexNode* current = graph->head;
        for (size_t i = 0; i < total_nodes; ++i) {
            VertexNode* next_node = current->next;
            vertex_dispose(current->v);
            free(current);
            current = next_node;
        }
    }
    free(graph);
}

static VertexNode* graph_find_node(Graph* graph, const char* name_of_person) {
    if (!graph || !graph->head || !name_of_person) return NULL;
    VertexNode* current = graph->head;
    for (size_t i = 0; i < graph->size; ++i) {
        if (current->v && current->v->name_of_person && strcmp(current->v->name_of_person, name_of_person) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

Vertex* graph_find_vertex(Graph* graph, const char* name_of_person) {
    VertexNode* node = graph_find_node(graph, name_of_person);
    return node ? node->v : NULL;
}

Vertex* graph_add_vertex(Graph* graph, const char* name_of_person) {
    if (!graph || !name_of_person) return NULL;
    VertexNode* existing = graph_find_node(graph, name_of_person);
    if (existing) return existing->v;

    Vertex* vertex = vertex_create(name_of_person);
    if (!vertex) return NULL;
    VertexNode* node = vertex_node_create(vertex);
    if (!node) { vertex_dispose(vertex); return NULL; }

    if (!graph->head) {
        node->next = node->prev = node;
        graph->head = node;
        graph->size = 1;
        return vertex;
    }
    VertexNode* tail = graph->head->prev;
    node->next = graph->head;
    node->prev = tail;
    tail->next = node;
    graph->head->prev = node;
    graph->size++;
    return vertex;
}

int graph_remove_vertex(Graph* graph, const char* name_of_person) {
    if (!graph || !name_of_person) return -1;
    VertexNode* node = graph_find_node(graph, name_of_person);
    if (!node) return -1;

    // 1) Удаляем саму вершину (инвалидирует handle, чистит её исходящие рёбра)
    vertex_dispose(node->v);
    node->v = NULL;

    // 2) Убираем входящие рёбра к этой вершине у остальных
    if (graph->head && graph->size > 0) {
        VertexNode* current = graph->head;
        for (size_t i = 0; i < graph->size; ++i) {
            if (current != node && current->v) {
                edge_list_remove_invalid(&current->v->edges, NULL);
            }
            current = current->next;
        }
    }

    // 3) Удаляем узел из списка графа
    graph_unlink_node(graph, node);
    free(node);
    return 0;
}

size_t graph_size(const Graph* graph) { return graph ? graph->size : 0; }
int    graph_is_empty(const Graph* graph) { return !graph || graph->size == 0; }

// =============================
// Общие хелперы для алгоритмов

static Vertex** graph_vertices_to_array(Graph* graph, size_t* out_count) {
    if (out_count) *out_count = 0;
    if (!graph || !graph->head || graph->size == 0) return NULL;
    Vertex** vertices = (Vertex**)malloc(sizeof(Vertex*) * graph->size);
    if (!vertices) return NULL;
    VertexNode* current = graph->head;
    for (size_t i = 0; i < graph->size; ++i) { vertices[i] = current->v; current = current->next; }
    if (out_count) *out_count = graph->size;
    return vertices;
}

static int vertex_index_in_array(Vertex** vertices, size_t count, Vertex* target) {
    for (size_t i = 0; i < count; ++i) if (vertices[i] == target) return (int)i;
    return -1;
}

static int positive_edge_exists(Vertex* from, Vertex* to) {
    if (!from || !to) return 0;
    Edge* head_edge = from->edges.head;
    if (!head_edge) return 0;
    Edge* edge_iter = head_edge;
    for (size_t i = 0; i < from->edges.size; ++i) {
        if (edge_iter->to && edge_iter->to->vertex == to && edge_iter->weight > 0)
            return 1;
        edge_iter = edge_iter->next;
    }
    return 0;
}

static void names_array_push(char*** names_out, size_t* names_count, const char* name) {
    char** grown = (char**)realloc(*names_out, (*names_count + 1) * sizeof(char*));
    if (!grown) return; // best effort
    *names_out = grown;
    (*names_out)[*names_count] = strdup(name ? name : "");
    if (!(*names_out)[*names_count]) return;
    (*names_count)++;
}

// =============================
// 1) BFS: знакомы не более чем через K рукопожатий
int graph_bfs_within_k(Graph* graph, const char* start_name, size_t max_hops, int positive_only,
                       char*** out_names, size_t* out_count) {
    if (out_names) *out_names = NULL;
    if (out_count) *out_count = 0;
    if (!graph || !start_name || !out_names || !out_count) return -1;

    VertexNode* start_node = graph_find_node(graph, start_name);
    if (!start_node) return -1;

    size_t total_vertices = 0;
    Vertex** vertices = graph_vertices_to_array(graph, &total_vertices);
    if (!vertices) return 0; // пустой граф

    int* distance = (int*)malloc(sizeof(int) * total_vertices);
    int* visited  = (int*)calloc(total_vertices, sizeof(int));
    int* queue    = (int*)malloc(sizeof(int) * total_vertices);
    if (!distance || !visited || !queue) { free(vertices); free(distance); free(visited); free(queue); return -1; }

    for (size_t i = 0; i < total_vertices; ++i) distance[i] = INT_MAX;

    int start_index = vertex_index_in_array(vertices, total_vertices, start_node->v);
    if (start_index < 0) { free(vertices); free(distance); free(visited); free(queue); return -1; }

    int q_head = 0, q_tail = 0;
    queue[q_tail++] = start_index;
    visited[start_index] = 1;
    distance[start_index] = 0;

    while (q_head < q_tail) {
        int current_index = queue[q_head++];
        if ((size_t)distance[current_index] == max_hops) continue; // не расширяем дальше

        Edge* head_edge = vertices[current_index]->edges.head;
        if (!head_edge) continue;
        Edge* edge_iter = head_edge;
        for (size_t step = 0; step < vertices[current_index]->edges.size; ++step) {
            if (!edge_iter->to || !edge_iter->to->vertex) { edge_iter = edge_iter->next; continue; }
            if (positive_only && edge_iter->weight <= 0) { edge_iter = edge_iter->next; continue; }
            int neighbor_index = vertex_index_in_array(vertices, total_vertices, edge_iter->to->vertex);
            if (neighbor_index >= 0 && !visited[neighbor_index]) {
                visited[neighbor_index] = 1;
                distance[neighbor_index] = distance[current_index] + 1;
                queue[q_tail++] = neighbor_index;
            }
            edge_iter = edge_iter->next;
        }
    }

    // Собираем имена всех вершин на расстоянии <= K, исключая стартовую
    char** result_names = NULL; size_t result_count = 0;
    for (size_t i = 0; i < total_vertices; ++i) {
        if ((int)i == start_index) continue;
        if (distance[i] != INT_MAX && (size_t)distance[i] <= max_hops) {
            names_array_push(&result_names, &result_count, vertices[i]->name_of_person);
        }
    }

    *out_names  = result_names;
    *out_count  = result_count;

    free(vertices); free(distance); free(visited); free(queue);
    return 0;
}

// =============================
// 2) Кратчайшая позитивная цепочка (Дейкстра)
static int positive_edge_cost(int weight, int cost_mode) {
    if (cost_mode == 0) return 1; // равные стоимости (кол-во звеньев)
    int bounded = weight; if (bounded < 1) bounded = 1; if (bounded > 10) bounded = 10;
    return 11 - bounded; // чем больше вес, тем дешевле
}

int graph_shortest_positive_chain(Graph* graph,
    const char* src_name,
    const char* dst_name,
    int cost_mode,
    char*** out_path_names,
    size_t* out_path_len,
    int* out_total_cost) {

    if (out_path_names) *out_path_names = NULL;
    if (out_path_len)   *out_path_len   = 0;
    if (out_total_cost) *out_total_cost = 0;
    if (!graph || !src_name || !dst_name || !out_path_names || !out_path_len) return -1;

    VertexNode* src_node = graph_find_node(graph, src_name);
    VertexNode* dst_node = graph_find_node(graph, dst_name);
    if (!src_node || !dst_node) return -1;

    size_t total_vertices = 0; Vertex** vertices = graph_vertices_to_array(graph, &total_vertices);
    if (!vertices) return 0;

    int src_index = vertex_index_in_array(vertices, total_vertices, src_node->v);
    int dst_index = vertex_index_in_array(vertices, total_vertices, dst_node->v);
    if (src_index < 0 || dst_index < 0) { free(vertices); return -1; }

    int* distance      = (int*)malloc(sizeof(int) * total_vertices);
    int* predecessor   = (int*)malloc(sizeof(int) * total_vertices);
    int* visited       = (int*)calloc(total_vertices, sizeof(int));
    if (!distance || !predecessor || !visited) { free(vertices); free(distance); free(predecessor); free(visited); return -1; }

    for (size_t i = 0; i < total_vertices; ++i){ distance[i] = INT_MAX; predecessor[i] = -1; }
    distance[src_index] = 0;

    for (size_t pass = 0; pass < total_vertices; ++pass) {
        int best_vertex = -1; int best_dist = INT_MAX;
        for (size_t i = 0; i < total_vertices; ++i) if (!visited[i] && distance[i] < best_dist) { best_dist = distance[i]; best_vertex = (int)i; }
        if (best_vertex == -1) break;
        visited[best_vertex] = 1;
        if (best_vertex == dst_index) break; // кратчайший найден

        Edge* head_edge = vertices[best_vertex]->edges.head;
        if (!head_edge) continue;
        Edge* edge_iter = head_edge;
        for (size_t step = 0; step < vertices[best_vertex]->edges.size; ++step) {
            if (!edge_iter->to || !edge_iter->to->vertex || edge_iter->weight <= 0) { edge_iter = edge_iter->next; continue; }
            int neighbor_index = vertex_index_in_array(vertices, total_vertices, edge_iter->to->vertex);
            if (neighbor_index < 0 || visited[neighbor_index]) { edge_iter = edge_iter->next; continue; }
            int cost = positive_edge_cost(edge_iter->weight, cost_mode);
            if (distance[best_vertex] != INT_MAX && distance[best_vertex] + cost < distance[neighbor_index]) {
                distance[neighbor_index]    = distance[best_vertex] + cost;
                predecessor[neighbor_index] = best_vertex;
            }
            edge_iter = edge_iter->next;
        }
    }

    if (distance[dst_index] == INT_MAX) { // пути нет
        free(vertices); free(distance); free(predecessor); free(visited);
        return 0; // корректный «пустой результат»
    }

    // восстановление пути
    size_t path_len = 0; int walk = dst_index;
    while (walk != -1) { path_len++; walk = predecessor[walk]; }

    char** path_names = (char**)malloc(sizeof(char*) * path_len);
    if (!path_names) { free(vertices); free(distance); free(predecessor); free(visited); return -1; }

    walk = dst_index;
    for (size_t i = 0; i < path_len; ++i) {
        path_names[path_len - 1 - i] = strdup(vertices[walk]->name_of_person);
        walk = predecessor[walk];
    }

    *out_path_names = path_names;
    *out_path_len   = path_len;
    if (out_total_cost) *out_total_cost = distance[dst_index];

    free(vertices); free(distance); free(predecessor); free(visited);
    return 0;
}

// =============================
// 3) Компоненты положительной достижимости

static int component_names_push(char*** names_array, size_t* count, const char* name){
    char** grown = (char**)realloc(*names_array, (*count + 1) * sizeof(char*));
    if (!grown) return -1;
    *names_array = grown;
    (*names_array)[*count] = strdup(name ? name : "");
    if (!(*names_array)[*count]) return -1;
    (*count)++;
    return 0;
}

static void dfs_directed_positive(Vertex** vertices, size_t count, int start_index, int* visited, int* order_indices){
    visited[start_index] = 1;
    Edge* head_edge = vertices[start_index]->edges.head;
    if (head_edge){
        Edge* edge_iter = head_edge;
        for (size_t step = 0; step < vertices[start_index]->edges.size; ++step){
            if (edge_iter->to && edge_iter->to->vertex && edge_iter->weight > 0){
                int neighbor_index = vertex_index_in_array(vertices, count, edge_iter->to->vertex);
                if (neighbor_index >= 0 && !visited[neighbor_index]) dfs_directed_positive(vertices, count, neighbor_index, visited, order_indices);
            }
            edge_iter = edge_iter->next;
        }
    }
    // append to order (find first -1 slot)
    size_t pos = 0; while (order_indices[pos] != -1) pos++;
    order_indices[pos] = start_index; order_indices[pos + 1] = -1;
}

static void dfs_directed_positive_transpose(Vertex** vertices, size_t count, int start_index, int* visited, char*** component_names, size_t* component_size){
    visited[start_index] = 1;
    component_names_push(component_names, component_size, vertices[start_index]->name_of_person);
    // По обратным рёбрам: ищем w с положительным ребром w->start_index
    for (size_t candidate = 0; candidate < count; ++candidate){
        if (visited[candidate]) continue;
        if (positive_edge_exists(vertices[candidate], vertices[start_index]))
            dfs_directed_positive_transpose(vertices, count, (int)candidate, visited, component_names, component_size);
    }
}

int graph_positive_components(Graph* graph, int directed_mode, GraphComponents** out_result){
    if (out_result) *out_result = NULL;
    if (!graph || !out_result) return -1;

    size_t total_vertices = 0; Vertex** vertices = graph_vertices_to_array(graph, &total_vertices);
    if (!vertices){
        GraphComponents* empty = (GraphComponents*)calloc(1, sizeof(GraphComponents));
        if (!empty) return -1;
        *out_result = empty; return 0;
    }

    GraphComponents* result = (GraphComponents*)calloc(1, sizeof(GraphComponents));
    if (!result){ free(vertices); return -1; }

    if (!directed_mode){
        // Слабосвязные компоненты на неориентированном положительном подграфе
        int* visited = (int*)calloc(total_vertices, sizeof(int));
        if (!visited){ free(vertices); free(result); return -1; }
        for (size_t start = 0; start < total_vertices; ++start){
            if (visited[start]) continue;
            char** component_names = NULL; size_t component_size = 0;
            // BFS по неориентированному положительному подграфу
            int* queue = (int*)malloc(sizeof(int) * total_vertices);
            int q_head = 0, q_tail = 0; queue[q_tail++] = (int)start; visited[start] = 1;
            while (q_head < q_tail){
                int current_index = queue[q_head++];
                component_names_push(&component_names, &component_size, vertices[current_index]->name_of_person);
                for (size_t other = 0; other < total_vertices; ++other){
                    if (visited[other]) continue;
                    if (positive_edge_exists(vertices[current_index], vertices[other]) ||
                        positive_edge_exists(vertices[other],    vertices[current_index])){
                        visited[other] = 1; queue[q_tail++] = (int)other;
                    }
                }
            }
            // Добавляем компоненту в результат
            size_t idx = result->num_components;
            size_t* new_sizes  = (size_t*)realloc(result->component_sizes, (idx + 1) * sizeof(size_t));
            char*** new_names  = (char***)realloc(result->names,          (idx + 1) * sizeof(char**));
            if (!new_sizes || !new_names){ /* OOM: best effort */ }
            result->component_sizes = new_sizes; result->names = new_names;
            result->component_sizes[idx] = component_size; result->names[idx] = component_names;
            result->num_components = idx + 1;
        }
        free(visited);
    } else {
        // Сильно связные компоненты положительного ориентированного подграфа (Косараджу)
        int* visited = (int*)calloc(total_vertices, sizeof(int));
        int* order_indices = (int*)malloc(sizeof(int) * (total_vertices + 1)); // -1-terminated
        if (!visited || !order_indices){ free(vertices); free(result); free(visited); free(order_indices); return -1; }
        for (size_t i = 0; i <= total_vertices; i++) order_indices[i] = -1;
        for (size_t i = 0; i < total_vertices; ++i) if (!visited[i]) dfs_directed_positive(vertices, total_vertices, (int)i, visited, order_indices);
        // второй проход по обратному графу
        memset(visited, 0, sizeof(int) * total_vertices);
        for (int pos = (int)total_vertices - 1; pos >= 0; --pos){
            int start_index = order_indices[pos]; if (start_index < 0) continue; if (visited[start_index]) continue;
            char** component_names = NULL; size_t component_size = 0;
            dfs_directed_positive_transpose(vertices, total_vertices, start_index, visited, &component_names, &component_size);
            size_t idx = result->num_components;
            size_t* new_sizes  = (size_t*)realloc(result->component_sizes, (idx + 1) * sizeof(size_t));
            char*** new_names  = (char***)realloc(result->names,          (idx + 1) * sizeof(char**));
            if (!new_sizes || !new_names){ /* OOM best-effort */ }
            result->component_sizes = new_sizes; result->names = new_names;
            result->component_sizes[idx] = component_size; result->names[idx] = component_names;
            result->num_components = idx + 1;
        }
        free(visited); free(order_indices);
    }

    *out_result = result;
    free(vertices);
    return 0;
}
