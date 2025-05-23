#ifndef DIALOGUE_H
#define DIALOGUE_H
#include "library.h"

int init_table(const Table* table);
int insert(const Table* table);
int search_key(Table* table);
int T_delete(Table* table);
int clear_table(Table* table);
void print_table(Table* table);

Table* file_import(Table* table,int* eof_tmp);

int special_search(Table* table);


#endif