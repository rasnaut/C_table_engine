#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lib/dialogue.h"
#include <readline/readline.h>

int main()
{
    Table* table = NULL;
    int eof_tmp = 0;
    if(init_table(table)==-1) return -1;

    printf("1.Inserting an element into a table\n");
    printf("2.Searching for an element in a table\n");
    printf("3.Removing an element from a table\n");
    printf("4.Outputting a table to the console\n");
    printf("5.Importing a table from a file\n");
    printf("6.Special operation - searching in a range:\n");

    int input = atoi(readline(""));

    switch (input)
    {
    case 1:
        if(insert(table) == -1) return -1;
        break;
    case 2:
        if(binary_search(table)==-1) return -1;
        break;
    case 3:
        if(T_delete(table) == -1) return -1;
        break;
    case 4:
        print_table(table);
        break;
    case 5:
        if(file_import(table,&eof_tmp)==NULL && eof_tmp == -1) return -1;
        break;
    case 6:
        if(special_search(table,&eof_tmp)==NULL && eof_tmp == -1) return -1;
        break;
    
    default:
        printf("Error. No such operation\n");
        break;
    }
    return 0;
}