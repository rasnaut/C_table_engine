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
    new_arr = key_array_realloc(new_arr, new_capacity);
    if(new_arr)
        new_arr->size = 0;
    
    return new_arr;
}

KeyArray* key_array_realloc(KeyArray* arr, size_t new_capacity) {
    if (!arr) return NULL;

    char** result = (char**)realloc(arr->array_ptr, new_capacity * sizeof(char*));
    if (!result) {
        fprintf(stderr, "Error: Memory allocation failed for result array\n");
        return NULL;
    }

    arr->array_ptr = result;
    arr->capacity = new_capacity;
    return arr;
}

int key_array_push_back(KeyArray* arr, const char* key) {
    if(!arr)
        return 1;
    
    if(arr->size >= arr->capacity) {
        arr = key_array_realloc(arr, 2 * arr->capacity);
        if(!arr)
            return 1;
    }
    arr->array_ptr[arr->size] = strdup(key);
    arr->size++; 
    return 0;   
}

void key_array_delete(KeyArray* arr) {
    if(arr) {
        for(size_t i = 0; i < arr->size; i++) {
            free(arr->array_ptr[i]);
            arr->array_ptr[i] = NULL;
        }   
    }
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