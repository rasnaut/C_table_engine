#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "library.h"

/*int getInt(int *x) 
{
    int count;
    char c;
    int temp;  

    while (1) {
        count = scanf("%d", &temp);  
        
        if (count == EOF) {
            printf("\nShutdown.\n");
            return -1;
        }

        if (count == 1) {
            if (temp <= 0) {
                printf("Error: Integer must be positive. Try again:");
            } 
            else if (temp > INT_MAX) {  
                printf("Error: Integer too large. Try again:");
            }
            else {
                *x = temp;
                
                if (scanf("%c", &c) == 1 && c != '\n') {
                    printf("Error: Extra characters detected. Try again:");
                    scanf("%*[^\n]");
                } 
                else {
                    return 0;
                }
            }
        } 
        else {
            printf("Error: Please enter a positive integer:");
            scanf("%*[^\n]");
            getchar();
        }
    }
}*/
char* getstr(FILE* stream) {
    char* line = NULL;
    size_t len = 0;
    ssize_t read = getline(&line, &len, stream);

    if (read == -1) {
        free(line);
        return NULL;
    }

    // Удаляем \n
    if (read > 0 && line[read - 1] == '\n') {
        line[--read] = '\0';
    }

    return line;
}

