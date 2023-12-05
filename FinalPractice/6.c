/**
 * Write a C program that will open a file (the name is given on the command line as argv[1]) that opens the file, 
 * seeks 1000 bytes into the file, seeks back to the beginning of the file, and then seeks to the end of the file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    exit(EXIT_FAILURE); }
    const char *filename = argv[1];
    int ifd = open(filename, O_RDONLY);

    if (ifd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    if (lseek(ifd, 1000, SEEK_SET) == -1) {
        perror("Error seeking into the file");
        close(ifd);
        exit(EXIT_FAILURE);
    }

    if (lseek(ifd, 0, SEEK_SET) == -1) {
        perror("Error seeking to the beginning of the file");
        close(ifd);
        exit(EXIT_FAILURE);
    }
    
    off_t fileSize = lseek(ifd, 0, SEEK_END);
    if (fileSize == -1) {
        perror("Error seeking to the end of the file");
        close(ifd);
        exit(EXIT_FAILURE);
    }

    close(ifd);
    return EXIT_SUCCESS;
}