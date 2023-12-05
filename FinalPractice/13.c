/**
 * Write a program that has a getopt() loop that processes command line options based on the following table:
 * -v: Toggles the variable v between 0 and 1, whose initial value is 0
 * -i int: Set the variable i to the value of the option int
 * -s str: Set the variable s (assumed to be large enough) to the value str
 * -f flt: Set the variable f to the value flt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

int main(int argc, char *argv[]) {
    int v = 0;
    int i = 0;
    char s[1024] = "";
    float f = 0.0;
    int opt;
    while ((opt = getopt(argc, argv, "vi:s:f:"))
            != -1) {
        switch (opt) {
            case 'v':
                // Toggle the variable v between 0 and 1
                v = 1 - v;
                break;
            case 'i':
                // Set the variable i to the value of the option int
                i = atoi(optarg);
                break;
            case 's':
                // Set the variable s to the value str
                snprintf(s, sizeof(s), "%s", optarg);
                break;
            case 'f':
                // Set the variable f to the value flt
                f = atof(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -v -i int -s str -f flt\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    printf("v: %d\n", v);
    printf("i: %d\n", i);
    printf("s: %s\n", s);
    printf("f: %f\n", f);

    return EXIT_SUCCESS;
}
