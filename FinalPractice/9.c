/**
 * Write a C program that creates a pipe and forks a child process that writes “Hello pipe” to the pipe, using the write() call. 
 * The parent process should read from the pipe using the read() call, displaying the data to the terminal.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int pipe_fd[2];
    pid_t pid;
    char buffer[1024];
    // Create a pipe
    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        close(pipe_fd[0]); // Close the read end of the pipe
        if (write(pipe_fd[1], "Hello pipe", sizeof("Hello pipe")) == -1) {
            perror("Write to pipe failed");
            exit(EXIT_FAILURE);
        }
        close(pipe_fd[1]); // Close the write end of the pipe in the child
        exit(EXIT_SUCCESS);
    } else {
        close(pipe_fd[1]); // Close the write end of the pipe in the parent
        ssize_t bytes_read = read(pipe_fd[0], buffer, sizeof(buffer));
        if (bytes_read == -1) {
            perror("Read from pipe failed");
            exit(EXIT_FAILURE);
        }
        printf("Parent received: %.*s\n", (int)bytes_read, buffer);
        close(pipe_fd[0]); // Close the read end of the pipe in the parent
        wait(NULL);
    }
    return EXIT_SUCCESS;
}
