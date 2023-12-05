/**
 * Write a C program that will read lines from standard-in (using fgets()), that will create a ragged array of the lines read. 
 * Note that you don’t know how many lines will be read from standard-in. 
 * You’ll need to use malloc()/realloc() . 
 * Make sure you also deallocate the memory from the ragged array.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main (void) {

    char **arr = NULL;
    char buff[1024];
    int lines = 0;

    while (fgets(buff, sizeof(buff), stdin) != NULL) {
        size_t len = strlen(buff);
        if (len > 0 && buff[len - 1] == '\n')
            buff[len - 1] = '\0';
        
        arr = realloc(arr, (lines + 1) * sizeof(char));
        arr[lines] = strdup(buff);
        lines++;
    }

    for (int i = 0; i < lines; i++) {
        printf("%03d: %s\n", (i), arr[i]);
        free(arr[i]);
    }

    free(arr);
    return EXIT_SUCCESS;
}