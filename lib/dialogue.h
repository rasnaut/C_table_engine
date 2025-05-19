#ifndef DIALOGUE_H
#define DIALOGUE_H
#include "library.h"

int init_table(const Table* table);
int insert(const Table* table);
int binary_search(Table* table);
int T_delete(Table* table);
void print_table(Table* table);

Table* file_import(Table* table,int* eof_tmp);
Table* special_search(Table* table,int* eof_tmp);

#endif