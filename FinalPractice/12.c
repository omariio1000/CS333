/**
 * Write a program that establishes signal handlers for SIGINT, SIGALRM, and SIGPIPE signals, using the signal() call
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

static volatile sig_atomic_t alarm_count = 5;
static volatile sig_atomic_t num_fork = 0;

void sig_handler(int sig) {
    if (sig == SIGALRM) {
        time_t t = time(NULL);
        alarm_count--;
        printf("The time is: %s", ctime(&t));
        if (alarm_count) {
            alarm(2);
        }
    } else if (sig == SIGINT) {
        printf("Received SIGINT signal: %d\n", sig);
        exit(EXIT_SUCCESS);
    } else if (sig == SIGPIPE) {
        printf("Received SIGPIPE signal: %d\n", sig);
    } else {
        // Handle other signals
    }
}


int main(void) {
    signal(SIGALRM, sig_handler);
    signal(SIGINT, sig_handler);
    signal(SIGPIPE, sig_handler);
    alarm(2);
    while (alarm_count || num_fork > 0) {
        pause();
    }
    return EXIT_SUCCESS;
}
