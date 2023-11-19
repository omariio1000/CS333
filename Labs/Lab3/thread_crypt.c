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
#include <crypt.h>
#include <unistd.h>

#include "thread_crypt.h"

#define MAX_FILE_NAME_LEN 20
#define MAX_FILE_LINE_LEN 128

void printHelp(char*);
void *encrypt(void *);
void readFile(void);

static FILE *input;
static FILE *output;
static int algo = 0;
static char *saltStr;
static char **salt;
static int saltLen = -1;
static int rounds = -1;
static char *roundsStr;
static int threadCount = 1;
static char saltChars [] = {SALT_CHARS};
static char **fileData;
static struct crypt_data *data;
static long lineCount;
static int *indexes;

int main(int argc, char *argv[]) {

    char fileInName[MAX_FILE_NAME_LEN];
    char fileOutName[MAX_FILE_NAME_LEN] = {0};
    int fileIn = 0;
    int fileOut = 0;
    int verbose = 0;
    unsigned int randSeed = 1; //default if nothing entered is srand(1)
    pthread_t *threads = NULL;
    long numCpus = sysconf(_SC_NPROCESSORS_ONLN);

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
                sscanf(optarg, "%d", &saltLen);
                break;
            case 'r':
                sscanf(optarg, "%d", &rounds);
                break;
            case 'R':
                sscanf(optarg, "%u", &randSeed);
                break;
            case 't':
                sscanf(optarg, "%d", &threadCount);
                if (threadCount > numCpus || threadCount < 1) {
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

    if (rounds == -1) rounds = 5000;
    else if (rounds < 1000) rounds = 1000;
    else if (rounds > 999999999) rounds = 999999999;

    srand(randSeed);

    //checking saltLen stuff
    switch(algo) {
    case 0: //DES
        saltLen = 2;
        rounds = 1;
        break;
    case 1: //MD5
        if (saltLen < 1 || saltLen > 8) 
            saltLen = 8;
        rounds = 1;
        break;
    case 5: //SHA256
        if (saltLen < 5 || saltLen > 16) 
            saltLen = 16;
        break;
    
    case 6: //SHA512
        if (saltLen < 6 || saltLen > 16) 
            saltLen = 16;
        break;
    default:
        fprintf(stderr, "i done goofed. my bad.\n");
        exit(EXIT_FAILURE);
    }

    roundsStr = malloc(17 * sizeof(char));
    if (algo == 5 || algo == 6) {
        sprintf(roundsStr, "rounds=%d$", rounds);
    }

    saltStr = malloc((saltLen + 1) * sizeof(char));
    saltStr[saltLen] = '\0';

    if (verbose) {
        fprintf(stderr, "> Algorithm: %d ", algo);
        switch (algo){
        case 0:
            fprintf(stderr, "(DES)\n");
            break;
        case 1:
            fprintf(stderr, "(MD5)\n");
            break;
        case 5:
            fprintf(stderr, "(SHA-256)\n");
            break;
        case 6:
            fprintf(stderr, "(SHA-512)\n");
            break;
        }
        fprintf(stderr, "> Salt length: %d\n", saltLen);
        if (algo == 5 || algo == 6) fprintf(stderr, "> Rounds: %d\n", rounds);
        fprintf(stderr, "> Rand seed: %u\n", randSeed);
        fprintf(stderr, "> Thread count: %d (%ld threads on system)\n", threadCount, numCpus);
        fprintf(stderr, "> Input file: %s\n", fileInName);
        fprintf(stderr, "> Output file: %s\n\n", fileOut ? fileOutName : "stdout");
    }

    salt = malloc(threadCount * sizeof(char*));
    for (int i = 0; i < threadCount; i++) salt[i] = malloc(CRYPT_MAX_PASSPHRASE_SIZE * sizeof(char));

    readFile();

    data = calloc(lineCount, sizeof(struct crypt_data));
    threads = malloc(threadCount * sizeof(pthread_t));
    indexes = malloc(threadCount * sizeof(int));

    for (long i = 0; i < threadCount; i++) {
        indexes[i] = i;
        pthread_create(&threads[i], NULL, encrypt, (void *) i);
    }

    for (long i = 0; i < threadCount; i++)
        pthread_join(threads[i], NULL);


    if (input) fclose(input);
    if (output) fclose(output);
    free(saltStr);
    free(roundsStr);
    free(threads);
    for (int i = 0; i < threadCount; i++) free(salt[i]);
    free(salt);
    free(indexes);
    free(data);
    for (int i = 0; i < lineCount; i++) free(fileData[i]);
    free(fileData);

    return EXIT_SUCCESS;
}

void printHelp(char *progName) {
    fprintf(stderr, "%s ...\n", progName);
    fprintf(stderr, "\tOptions: %s\n", OPTIONS);
    fprintf(stderr, "\t-i file\t\tinput file name (required)\n");
    fprintf(stderr, "\t-o file\t\toutput file name (default stdout)\n");
    fprintf(stderr, "\t-a #\t\talgorithm to use for hashing [0,1,5,6] (default 0 = DES)\n");
    fprintf(stderr, "\t-l #\t\tlength of saltLen (default 2 for DES, 8 for MD-5, 16 for SHA)\n");
    fprintf(stderr, "\t-r #\t\trounds to use for SHA-256, or SHA-512 (default 5000)\n");
    fprintf(stderr, "\t-R #\t\tseed for rand() (default none)\n");
    fprintf(stderr, "\t-t #\t\tnumber of threads to create (default 1)\n");
    fprintf(stderr, "\t-v\t\tenable verbose mode\n");
    fprintf(stderr, "\t-h\t\thelpful text\n");
}

void *encrypt(void *arg) {
    char *out;
    long idx = (long) arg;

    for (long thr = idx; thr < lineCount; thr += threadCount) {
        for (int i = 0; i < saltLen; i++) {
            int randInt = rand() % strlen(saltChars);
            saltStr[i] = saltChars[randInt];
        }
        if (algo != 0) sprintf(salt[idx], "$%d$%s%s$", algo, (roundsStr ? roundsStr : ""), saltStr);
        else sprintf(salt[idx], "%s", saltStr);
        
        data[thr].initialized = 0;

        out = crypt_rn(fileData[thr], salt[idx], (void*) &data[thr], sizeof(data[thr]));

        fprintf(output, "%s:%s\n", fileData[thr], out);
    }

    pthread_exit(EXIT_SUCCESS);
}

void readFile(void) {
    char c;
    while ((c = fgetc(input)) != EOF) {
        if (c == '\n')
            lineCount++;
    }
    
    fseek(input, 0, SEEK_SET);
    fileData = malloc(lineCount * sizeof(char*));
    for (int i = 0; i < lineCount; i++) {
        fileData[i] = malloc(MAX_FILE_LINE_LEN * sizeof(char));
        fgets(fileData[i], MAX_FILE_LINE_LEN, input);
        // fgets(data[i].input, CRYPT_MAX_PASSPHRASE_SIZE, input);
        fileData[i][strlen(fileData[i]) - 1] = '\0';
        // data[i].input[strlen(data[i].input) - 1] = '\0';
    }
}