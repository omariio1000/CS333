/**
 * Write a program that forks a child process that execs the 'ls -lF' program. 
 * The parent program should not allow a zombie process to be created.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        execlp("ls", "ls","-lF", (char *)NULL);
        // If execlp fails
        perror("Exec failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0); // Wait for the child to finish
        if (WIFEXITED(status)) {
            printf("Child process exited with status: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child process terminated by signal: %d\n", WTERMSIG(status));
        }
    }
    return EXIT_SUCCESS;
}
