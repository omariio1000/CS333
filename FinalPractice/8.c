/**
 * Write a C program that will monitor create, modify, and delete events for the directory given on the command line as argv[1].
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/inotify.h>

#define MAX_EVENTS 1024
#define LEN_NAME 40
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + LEN_NAME))

void monitor_directory(const char*);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *dir_to_monitor = argv[1];
    monitor_directory(dir_to_monitor);
    return EXIT_SUCCESS;
}

void monitor_directory(const char *dir_to_monitor) {
    int length, i = 0;
    int fd, wd;
    int create_events = 0;
    int event_count = 0;
    char buffer[BUF_LEN];
    fd = inotify_init();
    if (fd == -1) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }
    wd = inotify_add_watch(fd, dir_to_monitor, IN_CREATE | IN_MODIFY | IN_DELETE);
    if (wd == -1) {
        perror("add watch failed");
        exit(EXIT_FAILURE);
    } else {
        fprintf(stderr, "Watching: '%s'\n", dir_to_monitor);
    }
    for (;;) {
        length = read(fd, buffer, BUF_LEN);
        if (length == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            event_count++;
            if (event->len > 0) {
                if (event->mask & IN_CREATE) {
                    if (event->mask & IN_ISDIR) {
                        printf("A new directory %s was created.\n", event->name);
                    } 
                    else {
                        create_events++;
                        char *ext = strrchr(event->name, '.');
                        if (ext && strcasecmp(ext, ".xml") == 0) {
                            printf("It is an xml file.\n");
                        }
                    }
                } else if (event->mask & IN_MODIFY) {
                    if (event->mask & IN_ISDIR) {
                        printf("The directory %s was modified.\n", event->name);
                    } 
                    else {
                        printf("The file %s was modified.\n", event->name);
                    }
                } else if (event->mask & IN_DELETE) {
                    if (event->mask & IN_ISDIR) {
                        printf("The directory %s was deleted.\n", event->name);
                    } 
                    else {
                        printf("The file %s was deleted.\n", event->name);
                    }
                }
            }
            i += EVENT_SIZE + event->len;
        }
        i = 0; // Reset index for the next read
    }
    inotify_rm_watch(fd, wd);
    close(fd);
}
