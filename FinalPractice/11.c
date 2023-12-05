/**
 * Write a program that creates 4 threads using the PThreads API and joins with the threads when done. 
 * The thread function should sleep for 5 seconds before exiting.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void *threadFunction(void *arg) {
    sleep(5);
    pthread_exit(NULL);
}
int main() {
    pthread_t * threads = calloc(4, sizeof(pthread_t));
    // Create threads
    for (int i = 0; i < 4; ++i) {
        if (pthread_create(&threads[i], NULL, threadFunction, NULL) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i + 1);
            exit(EXIT_FAILURE);
        }
    }
    // Join threads
    for (int i = 0; i < 4; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Error joining thread %d\n", i + 1);
            exit(EXIT_FAILURE);
        }
    }
    free(threads);
    pthread_exit(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
