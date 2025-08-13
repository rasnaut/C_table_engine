#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lib_vertex.h"
#include "lib_vertex_handle.h"

VertexHandle* vertex_handle_create(Vertex *vertex)
{
    if (!vertex) {
        return NULL; // Invalid arguments
    }
    VertexHandle* handle = malloc(sizeof(VertexHandle));
    if (!handle) {
        fprintf(stderr, "Error: Memory allocation failed for VertexHandle\n");
        return NULL;
    }
    handle->vertex = vertex;
    handle->ref_count = 1; // Initialize reference count to 1
    return handle;
}

int vertex_handle_is_null(const VertexHandle *handle)
{
    if (!handle) {
        return 1; // Handle is NULL
    }
    
    return handle->vertex == NULL; // Check if vertex is NULL
}

void vertex_handle_invalidate(VertexHandle *handle)
{
    if (!handle) {
        return; // Nothing to invalidate
    }
    handle->vertex = NULL; // Invalidate the handle by setting vertex to NULL
    // Note: We do not change ref_count here, as it may still be used by other references
}

// +1 ссылка
int  vertex_handle_add_ref(VertexHandle* handle){
    if (!handle || !handle->vertex) {
        return -1; // Invalid handle or vertex
    }
    handle->ref_count++;
    return 0;
}

// -1 ссылка, если стало 0 — освобождает сам handle
int  vertex_handle_release(VertexHandle* handle){
    if (!handle) {
        return -1; // Invalid handle or vertex
    }
    if(handle->ref_count == 0) {
        fprintf(stderr, "Error: Attempt to release a handle with zero reference count\n");
        return -1; // Cannot release if ref_count is already zero
    }
    handle->ref_count--;
    if (handle->ref_count == 0) {
        free(handle); // Free the handle if reference count is zero
        return 0; // Successfully released
    }
    return handle->ref_count; // Still has references
}