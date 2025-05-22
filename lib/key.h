#ifndef KEY_H
#define KEY_H
#include "list.h"

typedef struct KeySpace {
    unsigned int busy; // флаг занятости
    char* key;         // строковый ключ
    size_t list_length;
    Node* node;        // указатель на цепочку значений
} KeySpace;


#endif