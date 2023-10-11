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

int main(int argc, char *argv[]) {
    int verbose = 0;

    {
        int opt = 0;
        while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
            switch(opt) {
            case 'v':
                verbose = 1;
                break;
            default:
                exit(EXIT_FAILURE);
                break;
            }
        }
    }

    return EXIT_SUCCESS;
}