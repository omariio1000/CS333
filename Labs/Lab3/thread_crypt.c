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

void printHelp(char*);

int main(int argc, char *argv[]) {

char fileInName[MAX_FILE_NAME_LEN];
char fileOutName[MAX_FILE_NAME_LEN] = {0};
int fileIn = 0;
int fileOut = 0;
int verbose = 0;
int algo = -1;
int salt = -1;
int rounds = 5000;
int randSeed = -1;
int threadCount = 1;
FILE *input;
FILE *output;

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
                fileOut = 1;
                break;
            case 'a':
                sscanf(optarg, "%d", &algo);
                if (algo != 0 && algo != 1 && algo != 5 && algo != 6) {
                    fprintf(stderr, "invalid hashing algorithm: %d\n", algo);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'l':
                sscanf(optarg, "%d", &salt);
                break;
            case 'r':
                sscanf(optarg, "%d", &rounds);
                break;
            case 'R':
                sscanf(optarg, "%d", &randSeed);
                break;
            case 't':
                sscanf(optarg, "%d", &threadCount);
                if (threadCount > 20 || threadCount < 1) {
                    fprintf(stderr, "invalid thread count %d\n", threadCount);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                verbose = 1;
                fprintf(stderr, "> verbose level: %d\n", verbose);
                break;
            case 'h':
                printHelp(argv[0]);
                exit(EXIT_SUCCESS);
            default:
                fprintf(stderr, "oopsie - unrecognized command line option \"(null)\"\n");
                break;
            }
        }
    }

    if (!fileIn) {
        fprintf(stderr, "must provide input file name\n");
        exit(EXIT_FAILURE);
    }
    else {
        input = fopen(fileInName, "r");
        if (input == NULL) {
            fprintf(stderr, "failed to open input file\n");
            exit(EXIT_FAILURE);
        }
    }
    if (fileOut) output = fopen(fileOutName, "w");
    else output = stdout;
    fprintf(output, "lol\n");

    if (rounds < 1000) rounds = 1000;
    if (rounds > 999999999) rounds = 999999999;

    //checking salt stuff
    if (algo == 0) {//DES
        if (salt < 0 || salt > 2) 
            salt = 2; 
    }
    else if (algo == 1) {//MD5
        if (salt < 1 || salt > 8) 
            salt = 8;
    }
    else if (algo == 5) {//SHA256
        if (salt < 5 || salt > 16) 
            salt = 16;
    }
    else if (algo == 6) {//SHA512
        if (salt < 6 || salt > 16) 
            salt = 16;
    }
    else {
        fprintf(stderr, "i done goofed. my bad.\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

void printHelp(char *progName) {
    fprintf(stderr, "%s ...\n", progName);
    fprintf(stderr, "\tOptions: %s\n", OPTIONS);
    fprintf(stderr, "\t-i file\t\tinput file name (required)\n");
    fprintf(stderr, "\t-o file\t\toutput file name (default stdout)\n");
    fprintf(stderr, "\t-a #\t\talgorithm to use for hashing [0,1,5,6] (default 0 = DES)\n");
    fprintf(stderr, "\t-l #\t\tlength of salt (default 2 for DES, 8 for MD-5, 16 for SHA)\n");
    fprintf(stderr, "\t-r #\t\trounds to use for SHA-256, or SHA-512 (default 5000)\n");
    fprintf(stderr, "\t-R #\t\tseed for rand() (default none)\n");
    fprintf(stderr, "\t-t #\t\tnumber of threads to create (default 1)\n");
    fprintf(stderr, "\t-v\t\tenable verbose mode\n");
    fprintf(stderr, "\t-h\t\thelpful text\n");
}