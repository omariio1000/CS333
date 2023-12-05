/* 
 * Write C program that reads from standard-in and writes to standard-out using the fgets() and fputs() calls.
 * Read and write all data until the end of data are encountered.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    char buffer[1024];
    FILE *ifp;
    ifp = stdin;
    
    if (ifp == NULL) {
        perror("Error reading from input");
        return EXIT_FAILURE;
    }

    while (fgets(buffer, 1024, ifp) != NULL)
        fputs(buffer, stdout);
    
    return EXIT_SUCCESS;
}