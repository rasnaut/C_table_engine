#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib_vertex.h"
#include "lib_edge.h"

static int clamp_w(int w){ if (w < -10) return -10; if (w > 10) return 10; return w; }

Vertex *vertex_create(const char *name_of_person)
{
    if (!name_of_person) {
        fprintf(stderr, "Error: Name of person cannot be NULL\n");
        return NULL;
    }
    Vertex *vertex = malloc(sizeof(Vertex));
    if (!vertex) {
        fprintf(stderr, "Error: Memory allocation failed for Vertex\n");
        return NULL;
    }

    vertex->name_of_person = strdup(name_of_person);
    if (!vertex->name_of_person) {
        fprintf(stderr, "Error: Memory allocation failed for name_of_person\n");
        free(vertex);
        return NULL;
    }

    edge_list_init(&vertex->edges);

    // ВАЖНО: создаём handle фабрикой и сразу связываем с вершиной
    vertex->handle = vertex_handle_create(vertex);
    if (!vertex->handle) {
        fprintf(stderr, "Error: Failed to create VertexHandle\n");
        free(vertex->name_of_person);
        free(vertex);
        return NULL;
    }
    return vertex;
}

void vertex_dispose(Vertex* vertex){
    if (!vertex) return;

    // Инвалидируем handle, чтобы все рёбра «увидели» смерть вершины
    if (vertex->handle) {
        vertex_handle_invalidate(vertex->handle);   // handle->vertex = NULL
    }

    // Удаляем все исходящие рёбра (список владеет узлами)
    edge_list_dispose(&vertex->edges);

    // Освобождаем +1 ссылку вершины на свой handle (release сам free'ит при 0)
    if (vertex->handle) {
        vertex_handle_release(vertex->handle);
        vertex->handle = NULL;
    }

    free(vertex->name_of_person);
    free(vertex);
}

int vertex_print(const Vertex* vertex){
    if (!vertex) {
        fprintf(stderr, "Error: Vertex is NULL\n");
        return -1;
    }
    printf("Vertex: %s\n", vertex->name_of_person);
    printf("Edges:\n");
    Edge* current = vertex->edges.head;
    if (!current) {
        printf("  No edges\n");
        return 0;
    }
    do {
        edge_print(current);
        current = current->next;
    } while (current != vertex->edges.head);
    return 0;
}

int vertex_add_edge(Vertex* from, VertexHandle* to, int weight) {
    if (!from || !to) {
        fprintf(stderr, "Error: Invalid arguments for adding edge\n");
        return -1;
    }

    Edge* new_edge = malloc(sizeof(Edge));
    if (!new_edge) {
        fprintf(stderr, "Error: Memory allocation failed for Edge\n");
        return -1;
    }

    if (edge_init(new_edge, to, clamp_w(weight)) != 0) {
        free(new_edge);
        return -1;
    }

    // ВАЖНО: upsert — если дубль, обновляем вес и освобождаем new_edge внутри
    int is_update = 0;
    int rc = edge_list_upsert(&from->edges, new_edge, &is_update);
    return rc; // 0 — успех
}

int vertex_remove_edge(Vertex* from, const VertexHandle* to) {
    if (!from || !to) {
        fprintf(stderr, "Error: Invalid arguments for removing edge\n");
        return -1;
    }
    // не смотрим на to->vertex; сравнение по адресу handle
    return edge_list_remove_by_handle(&from->edges, to);
}

int vertex_remove_edge_by_name(Vertex* from, const char* name_of_person) {
    if (!from || !name_of_person) {
        fprintf(stderr, "Error: Invalid arguments for removing edge by name\n");
        return -1;
    }
    Edge* e = edge_list_find_by_name(&from->edges, name_of_person);
    if (!e) {
        fprintf(stderr, "Error: Edge with name '%s' not found\n", name_of_person);
        return -1;
    }
    return edge_list_remove(&from->edges, e);
}