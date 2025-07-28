#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lib_key_array.h"

#define BASE_ARRAY_SIZE 10

KeyArray* key_array_create(){
    return key_array_create_with_capacity(BASE_ARRAY_SIZE);
}

KeyArray* key_array_create_with_capacity(size_t new_capacity) {
    KeyArray* new_arr = (KeyArray*)malloc(sizeof(KeyArray));
    new_arr->array_ptr = NULL;
    if(!key_array_realloc(new_arr, new_capacity))  {
        fprintf(stderr, "Error: Memory allocation failed for KeyArray\n");
        free(new_arr);
        return NULL;
    }
    new_arr->size = 0;
    printf("[ALLOC] KeyArray created at %p with capacity %zu\n", (void*)new_arr, new_capacity);

    return new_arr;
}

int key_array_realloc(KeyArray* arr, size_t new_capacity) {
    if (!arr) return 0;

    void** result = (void**)realloc(arr->array_ptr, new_capacity * sizeof(void*));
    if (!result) {
        fprintf(stderr, "Error: Memory allocation failed for result array\n");
        return 0;
    }

    arr->array_ptr = result;
    arr->capacity = new_capacity;
    return 1;
}

int key_array_push_back(KeyArray* arr, const char* key) {
    if(!arr)
        return 1;
    
    if(arr->size >= arr->capacity) {
        if(!key_array_realloc(arr, 2 * arr->capacity)) {
            fprintf(stderr, "Error: Memory allocation failed while resizing KeyArray\n");
            return 1;
        }
    }
    arr->array_ptr[arr->size] = strdup(key);
    arr->size++; 
    return 0;   
}

void key_array_delete(KeyArray* arr) {
    printf("[FREE] KeyArray deleted at %p\n", (void*)arr);

    if(arr) {
        for(size_t i = 0; i < arr->size; i++) {
            free(arr->array_ptr[i]);
            arr->array_ptr[i] = NULL;
        }   
        free(arr->array_ptr);
        arr->array_ptr = NULL;
    }
    arr->size = 0;
    arr->capacity = 0;
    free(arr);
}

void key_array_print(const KeyArray* arr) {
    if(arr) {
        printf("KeyArray: size = %zu, capacity = %zu\n", arr->size, arr->capacity);
        for(size_t i = 0; i < arr->size; i++) {
            printf("  [%zu]: %s\n", i, arr->array_ptr[i]);
        }
    } else {
        printf("KeyArray is NULL\n");
    }
}