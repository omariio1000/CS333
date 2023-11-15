/* Omar Nassar
 * November 14, 2023
 * Portland State University CS333
 * Program to encrypt text
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>

#include "thread_crypt.h"

#define MAX_FILE_NAME_LEN 20

void printHelp(void);

int main(int argc, char *argv[]) {

char fileInName[MAX_FILE_NAME_LEN];
char fileOutName[MAX_FILE_NAME_LEN];
int fileIn = 0;
int fileOut = 0;
int verbose = 0;

    {
        int opt = 0;
        while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
            switch(opt) {
            case 'i':
                strcpy(fileInName, optarg);
                fileIn = 1;
                break;
            case 'o':
                strcpy(fileOutName, optarg);
                fileIn = 1;
                break;
            case 'a':
                break;
            case 'l':
                break;
            case 'r':
                break;
            case 'R':
                break;
            case 't':
                break;
            case 'v':
                verbose = 1;
                fprintf(stderr, "> verbose level: %d\n", verbose);
                break;
            case 'h':
                printHelp();
                break;
            default:
                fprintf(stderr, "%s: invalid option -- '%c'", argv[0], optarg);
                fprintf(stderr, "oopsie - unrecognized command line option \"(null)\"\n");
                break;
            }
        }
    }

    return EXIT_SUCCESS;
}

void printHelp(void) {
    fprintf(stderr, "help text");
    
}