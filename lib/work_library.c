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

char *getstr(FILE *stream)
{
   char buf[81] = {0};
   char *res = NULL;
   int len = 0;
   int n = 0;
   void *check_mem;

   do {
      n = fscanf(stream, "%80[^\n]", buf);
      if ((n < 0) && (!res)){
        return NULL;
    }
    else if (n > 0) {
       int chunk_len =(int)strlen(buf);
       int str_len = len + chunk_len;
       res = realloc(res, str_len + 1);
       if (!res)
       {
          return NULL;
       }
   
       check_mem = memcpy(res + len, buf, chunk_len);
       if (check_mem != res+len){
          return NULL;
       }
       len = str_len;
    }
    else {
      fscanf(stream, "%*c");
    }
   } while (n > 0);

   if (len > 0) {
      res[len] = '\0';
   }
   else {
      res = calloc(1, sizeof(char));
      if (!res) {
        return NULL;
      }
   }
   return res;
}