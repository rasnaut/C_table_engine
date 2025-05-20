#include "key.h"
#include <stdlib.h>   // free, malloc, NULL
#include <string.h> 

// KeySpace keyspace_create(const char* key, unsigned int info) {
//     KeySpace ks;
//     if (!key) return ks;

//     ks.key = strdup(key);
//     if (!ks.key) return ks;

//     ks.info = info;
//     return ks;
// }

void keyspace_destroy(KeySpace* ks) {
    if (!ks) return;

    free(ks->key);
    ks->key = NULL;
}