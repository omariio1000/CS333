/**
 * Write C program that reads from standard-in and writes to standard-out using the read() and write() calls. 
 * Read and write all data until the end of data are encountered.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    char buffer[1024];
    ssize_t rez;
    int ifd;
    ifd = STDIN_FILENO;

    while ((rez = read(ifd, buffer, sizeof(buffer))) > 0) {
        if (write(STDOUT_FILENO, buffer, rez) != rez) {
            perror("Error writing to standard output");
            return 1;
        }
    }
    
    if (rez < 0) {
        perror("Error reading from input");
        return 1;
    }
    close(ifd);
    
    return EXIT_SUCCESS; 
}