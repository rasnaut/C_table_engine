#ifndef LIB_VERTEX_HANDLER
#define LIB_VERTEX_HANDLER

#include <stddef.h> // size_t

struct Vertex; // fwd

// ----- VertexHandle (proxy) -----
typedef struct VertexHandle {
    struct Vertex* vertex;   // NULL => вершина инвалидирована/удалена
    size_t ref_count;        // счётчик ссылок
} VertexHandle;

// Инициализирует handle для существующей вершины (ref_count = 1)
VertexHandle* vertex_handle_create(struct Vertex* vertex);

int vertex_handle_is_null(const VertexHandle* handle);  // true, если handle не указывает на живую вершину
void vertex_handle_invalidate(VertexHandle* handle);    // Помечает handle как инвалидный (vertex=NULL). Счётчик не меняется.

// +1 ссылка
int  vertex_handle_add_ref(VertexHandle* handle);

// -1 ссылка, если стало 0 — освобождает сам handle
int  vertex_handle_release(VertexHandle* handle);

#endif // LIB_VERTEX_HANDLER
