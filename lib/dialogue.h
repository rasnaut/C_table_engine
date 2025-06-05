#ifndef DIALOGUE_H
#define DIALOGUE_H
#include "library.h"
#include "lib_binary_tree.h"

int init_table(Table* table);
int insert(Table* table);
int search_key(Table* table);
int T_delete(Table* table);
int clear_table(Table* table);
void print_table(Table* table);

Table* file_import(Table* table,int* eof_tmp);

int special_search(Table* table);

BinaryTree* create_binary_tree();
int insert_node_to_binary_tree(BinaryTree* tree);
int search_key_in_binary_tree(BinaryTree* tree);
int delete_node_in_binary_tree(BinaryTree* tree);
int clear_binary_tree(BinaryTree* tree);
void print_binary_tree(BinaryTree* tree);
void print_tree_view_binary_tree(BinaryTree* tree);
BinaryTree* binary_tree_file_import(int* eof_tmp);
int special_search_in_binary_tree(BinaryTree* tree);
int generate_graphviz_file(const BinaryTree* tree);

#endif