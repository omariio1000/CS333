/**
 * Write a C program that will read lines from standard-in (using fgets()), that will strtok() each of the lines read and print each word to standard-out. 
 * Note that you don’t know how many lines will be read from standard-in.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    char buffer[1024];
    
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        char *token = strtok(buffer, " \t\n"); 
        while (token != NULL) {
            printf("%s\n", token);
            token = strtok(NULL, " \t\n");
        }
    }
    return EXIT_SUCCESS;
}