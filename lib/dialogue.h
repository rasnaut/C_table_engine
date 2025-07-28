#ifndef DIALOGUE_H
#define DIALOGUE_H
#include "lib_binary_tree.h"

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