/**
 * Write a C program that will print each of the values given in the argv argument to main().
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    for (int i = 0; i < argc; ++i) 
        printf("argv[%d]: %s\n", i, argv[i]);
    
    return EXIT_SUCCESS;
}