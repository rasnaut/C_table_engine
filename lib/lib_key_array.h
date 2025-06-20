#ifndef LIB_KEY_ARRAY_H
#define LIB_KEY_ARRAY_H

typedef struct KeyArray {
    char** array_ptr;
    size_t size;
    size_t capacity;
} KeyArray;

KeyArray* key_array_create();
KeyArray* key_array_create_with_capacity(size_t new_capacity);
int key_array_realloc(KeyArray* arr, size_t new_capacity);
int key_array_push_back(KeyArray* arr, const char* key);
void key_array_delete(KeyArray* arr);
void key_array_print(const KeyArray* arr);

#endif // LIB_KEY_ARRAY_H
