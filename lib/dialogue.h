#ifndef DIALOGUE_H
#define DIALOGUE_H
#include "lib_tree_234.h"

Node234* create_234_tree();
int insert_node_to_234_tree(Node234** root);
int search_key_in_234_tree(Node234* root);
int delete_node_in_234_tree(Node234** root);
int clear_234_tree(Node234** tree);
void print_tree_view_234_tree(Node234* root);
void print_sort_view_234_tree(Node234* root);
Node234* node_234_tree_file_import(int* eof_tmp);
int special_search_in_234_tree(Node234* root);
int generate_graphviz_file_234_tree(const Node234* root);

#endif