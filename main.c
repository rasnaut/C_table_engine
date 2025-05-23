#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lib/dialogue.h"
#include <readline/readline.h>

int main()
{
    Table* table = malloc(sizeof(Table));;
    int eof_tmp = 0;
    if(init_table(table)==-1) {
        free(table);
        return -1;
    }
    while(1) {
        printf("1 - Inserting an element into a table\n");
        printf("2 - Searching for an elements in a table\n");
        printf("3 - Removing an element by key and release from a table\n");
        printf("4 - Outputting a table to the console\n");
        printf("5 - Importing a table from a file\n");
        printf("6 - Searching for an element by key and release in a table\n");
        printf("7 - Exit\n");

        char* input_str = readline("Enter comand number: ");
        int input = atoi(input_str);
        free(input_str);

        switch (input)
        {
        case 1:
            if(insert(table) == -1) return -1;
            break;
        case 2:
            if(search_key(table)==-1) return -1;
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
            special_search(table);
            return 0;
        case 7:
            free_table(table);
            return 0;
        default:
            printf("Error. No such operation\n");
            break;
        }
    }
    free_table(table);
    return 0;
}