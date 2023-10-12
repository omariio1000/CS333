/* Omar Nassar
 * October 10, 2023
 * Portland State University CS333
 * Program to create archives of files
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <getopt.h>

#include "viktar.h"

void printHelp(void);

int main(int argc, char *argv[]) {
    FILE *file = NULL;
    char filename[FILENAME_MAX];
    int verbose = 0;
    int extractMode = 0;
    int createMode = 0;
    int shortTOC = 0;
    int longTOC = 0;

    

    {
        int opt = 0;
        while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
            switch(opt) {
            case 'v':
                verbose = 1;
                fprintf(stderr, "verbose level: %d\n", verbose);
                break;
            case 'h':
                printHelp();
                exit(EXIT_SUCCESS);
                break;
            case 'f':
                strcpy(filename, optarg);
                file = fopen(filename, "rw");
                if (file == NULL) {
                    fprintf(stderr, "Failed to open file %s\n", filename);
                    fprintf(stderr, "file open failed: No such file or directory\n");
                    return(EXIT_FAILURE);
                }
                fclose(file);
                break;
            case 'c':
                createMode = 1;
                break;
            case 't':
                shortTOC = 1;
                break;
            case 'T':
                longTOC = 1;
                break;
            case 'x':
                extractMode = 1;
                break;
            default:
                fprintf(stderr, "oopsie - unrecognized command line option \"(%c)\"\n", optarg);
                break;
            }
        }
    }

    if (!(extractMode || createMode || shortTOC || longTOC)) {
        fprintf(stderr, "no action supplied\n");
        fprintf(stderr, "exiting without doing ANYTHING...\n");
        exit(EXIT_FAILURE);
    }

    if (!(extractMode && createMode && shortTOC && longTOC)) {
        fprintf(stderr, "too many options supplied\n");
        fprintf(stderr, "exiting without doing ANYTHING...\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

void printHelp(void) {
    fprintf(stderr, "help text\n");
    fprintf(stderr, "\t./viktar\n");
    fprintf(stderr, "\t\t-x\t\textract file/files from archive\n");;
    fprintf(stderr, "\t\t-c\t\tcreate an archive file\n");;
    fprintf(stderr, "\t\t-t\t\tdisplay a short table of contents of the archive file\n");;
    fprintf(stderr, "\t\t-T\t\tdisplay a long table of contents of the archive file\n");;
    fprintf(stderr, "\t\tOnly one of xctT can be specified\n");
    fprintf(stderr, "\t\t-f filename\tuse filename as the archive file\n");
    fprintf(stderr, "\t\t-v\t\tgive verbose diagnostic messages\n");
    fprintf(stderr, "\t\t-h\t\tdisplay this AMAZING help message\n");
}