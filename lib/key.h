#ifndef KEY_H
#define KEY_H

typedef struct KeySpace {
    char* key;
    unsigned int info; // зачем хранить указатель, если можно хранить значение? размеры идентичны
    } KeySpace;


KeySpace* keyspace_create(KeySpace* ks, const char* key, unsigned int info);
void keyspace_destroy(KeySpace* ks);


#endif