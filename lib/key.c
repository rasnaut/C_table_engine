#include "key.h"
#include <stdlib.h>   // free, malloc, NULL
#include <string.h> 

KeySpace* keyspace_create(KeySpace* ks, const char* key, unsigned int info) {
    if (!ks || !key) return NULL;

    ks->key = strdup(key);
    if (!ks->key) return NULL;

    ks->info = info;
    return 0;
}

void keyspace_destroy(KeySpace* ks) {
    if (!ks) return;

    free(ks->key);
    ks->key = NULL;
}