#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lib/dialogue.h"
#include <readline/readline.h>

int main()
{
    int eof_tmp = 0;
    BinaryTree* tree = NULL;
    while(1) {
        unsigned int point_number = 1;
        printf("%d - create tree\n", point_number++);
        printf("%d - insert new element to tree\n", point_number++);
        printf("%d - Removing an element by key and release from a tree\n", point_number++);
        printf("%d - Outputting a tree to the console sort view\n", point_number++);
        printf("%d - Outputting a tree to the console tree view\n", point_number++);
        printf("%d - Importing a tree from a file\n", point_number++);
        printf("%d - Searching for an element by key in tree\n", point_number++);
        printf("%d - Show all elements which not contain key\n", point_number++);
        printf("%d - Generate graphiviz file\n", point_number++);
        printf("%d - Exit\n", point_number++);

        char* input_str = readline("Enter comand number: ");
        int input = atoi(input_str);
        free(input_str);

        switch (input)
        {
        case 1:
            tree = create_binary_tree();
            if(tree == NULL) return -1;
            break;
        case 2:
            if(insert_node_to_binary_tree(tree) == -1) return -1;
            break;
        case 3:
            if(delete_node_in_binary_tree(tree) == -1) return -1;
            break;
        case 4:
            binary_tree_print_sort_view(tree);
            break;
        case 5:
            print_tree_view_binary_tree(tree);
            break;
        case 6:
            binary_tree_file_import(tree, &eof_tmp);
            if (eof_tmp == -1) {    
                printf("Error: File not found or invalid data entry\n");
                return -1;
            }
            break;
        case 7:
            search_key_in_binary_tree(tree);
            break;
        case 8:
            special_search_in_binary_tree(tree);
            break;
        case 9:
            generate_graphviz_file(tree);
            break;
        case 10:
            if (tree) {
                binary_tree_destroy(tree);
                tree = NULL;
            }
            printf("Exiting...\n");
            return 0;
        default:
            printf("Error. No such operation\n");
            break;
        }
    }
    if(tree) 
        binary_tree_destroy(tree);
    
    return 0;
}