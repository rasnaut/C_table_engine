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

char *getstr(FILE *stream) {
    char buf[81] = {0};
    char *res = NULL;
    int len = 0;
    int n = 0;

    while (1) {
        n = fscanf(stream, "%80[^\n]", buf);
        if (n == EOF && !res) {
            return NULL;
        } else if (n > 0) {
            int chunk_len = (int)strlen(buf);
            int new_len = len + chunk_len;
            char *tmp = realloc(res, new_len + 1);
            if (!tmp) {
                free(res);
                return NULL;
            }
            res = tmp;
            memcpy(res + len, buf, chunk_len);
            len = new_len;
        }

        int ch = getc(stream);
        if (ch == '\n' || ch == EOF)
            break;
    }

    if (res) {
        res[len] = '\0';
    } else {
        res = calloc(1, sizeof(char));
    }

    return res;
}
