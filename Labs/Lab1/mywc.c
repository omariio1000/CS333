/* Omar Nassar
 * October 1, 2023
 * Portland State University CS333
 * Program to count chars, words, and lines of files
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    FILE *file = stdin;
    char *filename = NULL;
    char buf[BUFFER_SIZE] = {0};

    int dispChar = 0;
    int dispLine = 0;
    int dispWord = 0;
    int verbose = 0;

    int charCount = 0;
    int lineCount = 0;
    int wordCount = 0;


    {
        int opt = 0;

       while ((opt = getopt(argc, argv, "clwf:hv")) != -1) {
            switch(opt) {
            case 'c':
                dispChar = 1;
                break;
            case 'l':
                dispLine = 1;
                break;
            case 'w':
                dispWord = 1;
                break;
            case 'f':
                file = fopen(optarg, "r");
                filename = optarg;
                if (file == NULL) {
                    fprintf(stderr, "Failed to open file %s\n", filename);
                    fprintf(stderr, "file open failed: No such file or directory\n");
                    return(EXIT_FAILURE);
                }
                break;
            case 'h':
                printf("./mywc");
                printf("\toptions: clwf:hv");
                printf("\t-c\t: display the number of characters in the input");
                printf("\t-l\t: display the number of lines in the input");
                printf("\t-w\t: display the number of words in the input");
                printf("\t-f file\t: use file as input, defaults to stdin");
                printf("\t-h\t: display a command options and exit");
                printf("\t-v\t: give LOTS of gross verbose trace output to stderr.");

                exit(EXIT_SUCCESS);
                break;
            case 'v':
                verbose = 1;
                break;
            default:
                exit(EXIT_FAILURE);
            }
        }
    }

    while(fgets(buf, BUFFER_SIZE, file) != NULL) {
        int localCharCount = 0;
        
        int i = 0;
        while (buf[i] != '\0') {
            localCharCount++;
            if (buf[i] == ' ') wordCount++;
            if (buf[i] == '\n') {
                wordCount++;
                lineCount++;
            }
            i++;
        }
        if (verbose) printf("DEBUG: line <<%s>> %d\n", buf, localCharCount);
        charCount += localCharCount;
    }

    if (!dispLine && !dispWord && !dispChar) dispLine = dispWord = dispChar = 1;

    if (dispLine) printf("%d ", lineCount);
    if (dispWord) printf("%d ", wordCount);
    if (dispChar) printf("%d ", charCount);
    if (file != stdin) printf("%s", filename);
    printf("\n");

    if (file != stdin) fclose(file);

    return EXIT_SUCCESS;
}