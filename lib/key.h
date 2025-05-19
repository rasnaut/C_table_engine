#ifndef KEY_H
#define KEY_H

typedef struct KeySpace {
    char* key;
    unsigned int info; // зачем хранить указатель, если можно хранить значение? размеры идентичны
    } KeySpace;


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


#endif