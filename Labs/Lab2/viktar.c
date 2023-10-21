/* Omar Nassar
 * October 10, 2023
 * Portland State University CS333
 * Program to create archives of files
 */


#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <getopt.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
#include <utime.h>

#include "viktar.h"

#define MAX_FILE_NAME 100


void printHelp(void);
int printContents(char*, int, int, int);
void strmode(mode_t, char*);
char getType (mode_t mode);
int getFd(char*, int, int, int);
int checkVik(char*, int);
void parseFiles(int, char**, char***);
void createVik(char**, int, char*, int);
void extractVik(char**, int, char*, int);
int checkFileName(char**, int, char*);

int verbose = 0;

int main(int argc, char *argv[]) {
    char filename[VIKTAR_MAX_FILE_NAME_LEN];
    int extractMode = 0;
    int createMode = 0;
    int shortTOC = 0;
    int longTOC = 0;
    int fileIn = 0;

    {
        int opt = 0;
        while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
            switch(opt) {
            case 'v':
                verbose = 1;
                fprintf(stderr, "> verbose level: %d\n", verbose);
                break;
            case 'h':
                printHelp();
                exit(EXIT_SUCCESS);
                break;
            case 'f':
                strcpy(filename, optarg);
                fileIn = 1;
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
                fprintf(stderr, "oopsie - unrecognized command line option \"(null)\"\n");
                break;
            }
        }
    }

    if (verbose) for (int i = 0; i < argc; i++) fprintf(stderr, "%s%s", argv[i], (i < argc - 1) ? ", " : "\n");

    if (!(extractMode || createMode || shortTOC || longTOC)) {
        fprintf(stderr, "no action supplied\n");
        fprintf(stderr, "exiting without doing ANYTHING...\n");
        exit(EXIT_FAILURE);
    }

    if ((extractMode + createMode + shortTOC + longTOC) > 1) {
        fprintf(stderr, "too many options supplied\n");
        fprintf(stderr, "exiting without doing ANYTHING...\n");
        exit(EXIT_FAILURE);
    }

    

    if (shortTOC || longTOC) {
        int ifd = 0;
        int ret = 0;
        if (verbose) fprintf(stderr, "> Printing %s table of contents.\n", shortTOC ? "short" : "long");
        if (!fileIn) fprintf(stderr, "archive from stdin\n");

        ifd = checkVik(filename, fileIn);
        if (verbose) fprintf(stderr, "> Input file descriptor: %d\n", ifd);

        ret = printContents(filename, longTOC, ifd, fileIn);
        if (ret == -1) fprintf(stderr, "some sorta error");
    }

    if (createMode || extractMode) {
        int total = argc - 1;
        char **files;
        int offset = 0;
        if (verbose) fprintf(stderr, "> %s mode\n", createMode ? "Create": "Extract");
        
        for (int i = 0; i < total; i++) if (argv[i][0] == '-') total --;
        if (fileIn) total--;

        offset = argc - total;

        if (verbose) fprintf(stderr, "> ARGC: %d, TOTAL: %d, OFFSET: %d\n", argc, total, offset);

        if (total) {
            files = calloc(total, sizeof(char*));
            for (int i = 0; i < total; i++) {
                files[i] = calloc(MAX_FILE_NAME, sizeof(char));
                strcpy(files[i], argv[i + offset]);
                if (verbose) fprintf(stderr, "> Filename parsed: %d (%d): %s\n", i, i + offset, files[i]);
            }
        }

        if (createMode) createVik(files, total, filename, fileIn);
        else extractVik(files, total, filename, fileIn);

        for (int i = 0; i < total; i++) free(files[i]);
        if (total) free(files);
    }

    return EXIT_SUCCESS;
}

void printHelp(void) {
    fprintf(stderr, "help text\n");
    fprintf(stderr, "\tOptions: %s\n", OPTIONS);
    fprintf(stderr, "\t\t-x\t\textract file/files from archive\n");;
    fprintf(stderr, "\t\t-c\t\tcreate an archive file\n");;
    fprintf(stderr, "\t\t-t\t\tdisplay a short table of contents of the archive file\n");;
    fprintf(stderr, "\t\t-T\t\tdisplay a long table of contents of the archive file\n");;
    fprintf(stderr, "\t\tOnly one of xctT can be specified\n");
    fprintf(stderr, "\t\t-f filename\tuse filename as the archive file\n");
    fprintf(stderr, "\t\t-v\t\tgive verbose diagnostic messages\n");
    fprintf(stderr, "\t\t-h\t\tdisplay this AMAZING help message\n");
}

int printContents(char *fileName, int longTOC, int ifd, int file) {
    
    viktar_header_t viktar = {0};
    int totalFiles = 0;
   
    if (!file) strcpy(fileName, "stdin");
    fprintf(stdout, "Contents of viktar file: \"%s\"\n", fileName);
    while(read(ifd, &viktar, sizeof(viktar_header_t)) > 0) {
        printf("\tfile name: %s\n", viktar.viktar_name);
        if (longTOC) {
            char mode[10];
            char fileType = getType(viktar.st_mode);
            struct passwd *pwd = getpwuid(viktar.st_uid);
            struct group *grp = getgrgid(viktar.st_gid); 
            char timeStr[100];
            strmode(viktar.st_mode, mode);
            if (viktar.st_mode & S_ISGID) mode[5] = 'S';

            printf("\t\tmode:  %c%s\n", fileType, mode);
            printf("\t\tuser:  %s\n", (pwd != NULL) ? pwd->pw_name : "unknown");
            printf("\t\tgroup: %s\n", (grp != NULL) ? grp -> gr_name : "unknown");
            printf("\t\tsize:  %ld\n", viktar.st_size);
            
            strftime(timeStr, 100, "%Y-%m-%d %H:%M:%S %Z", localtime(&viktar.st_mtim.tv_sec));
            printf("\t\tmtime: %s\n", timeStr);

            strftime(timeStr, 100, "%Y-%m-%d %H:%M:%S %Z", localtime(&viktar.st_atim.tv_sec));
            printf("\t\tatime: %s\n", timeStr);

        }
        lseek(ifd, viktar.st_size, SEEK_CUR);
        totalFiles++;
    }
    if (verbose) fprintf(stderr, "> %d total files\n", totalFiles);

    close(ifd);
    return 1;
}

void strmode(mode_t mode, char * buf) {
  const char chars[] = "rwxrwxrwx";
  for (size_t i = 0; i < 9; i++) {
    buf[i] = (mode & (1 << (8-i))) ? chars[i] : '-';
  }
  buf[10] = '\0';
}

char getType (mode_t mode) {
    switch (mode & __S_IFMT) {            //bitwise AND to determine file type
        case __S_IFSOCK:  return 's';     //socket
        case __S_IFLNK:   return 'l';     //symbolic link
        case __S_IFREG:   return '-';     //regular file
        case __S_IFBLK:   return 'b';     //block device
        case __S_IFDIR:   return 'd';     //directory
        case __S_IFCHR:   return 'c';     //char device
        case __S_IFIFO:   return 'p';     //pipe
        default:          return '?';     //unknown
    }
}

int getFd(char* fileName, int file, int output, int chmod) {
    int fd = -1;
    if (verbose) fprintf(stderr, "> Getting %s file descriptor\n", output ? "output" : "input");
    if (chmod == -1) chmod = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    if (file) {
        if (output) fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, chmod);
        else fd = open(fileName, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "failed to open input archive file \"%s\"\n", fileName);
            fprintf(stderr, "exiting...\n");
            exit(EXIT_FAILURE);
        }
    }
    else {
        if (output) fd = dup(1);
        else fd = dup(0);
    }

    return fd;
}

int checkVik(char *fileName, int file) {
    char temp[10] = {0};
    int ifd = -1; 
    
    if (verbose) fprintf(stderr, "> Checking if \"%s\" is a viktar file\n", fileName);

    ifd = getFd(fileName, file, 0, -1);;

    if (read(ifd, &temp, sizeof(temp)) > 0) {
        if (strcmp(temp, VIKTAR_FILE) == 0) return ifd;
        else {
            if (strcmp(fileName, "") == 0) strcpy(fileName, "(null)");
            fprintf(stderr, "not a viktar file \"%s\"\n", fileName);
            fprintf(stderr, "\tthis is vik-terrible\n");
            fprintf(stderr, "\texiting...\n");
            exit(EXIT_FAILURE);
        }
    }

    return -1;
}

void createVik(char** files, int numFiles, char *fileName, int file) {
    int ofd = getFd(fileName, file, 1, -1);
    mode_t old_mode = 0;
    old_mode = umask(0);
    write(ofd, VIKTAR_FILE, sizeof(VIKTAR_FILE) - 1);

    if (verbose) fprintf(stderr, "> Output file descriptor: %d\n", ofd);

    for (int i = 0; i < numFiles; i++) {
        viktar_header_t viktar;
        int ifd = -1;
        int bytesRead;
        int bytesWrite;
        struct stat statbuf;
        int ret = stat(files[i], &statbuf);
        void *data = malloc(statbuf.st_size*sizeof(char));
        if (ret == -1) {
            fprintf(stderr, "failed to stat file \"%s\"\n", files[i]);
            fprintf(stderr, "exiting...\n");
            exit(EXIT_FAILURE);
        }

        memset(&viktar, 0, sizeof(viktar_header_t));
        strncpy(viktar.viktar_name, files[i], VIKTAR_MAX_FILE_NAME_LEN);

        viktar.st_mode = statbuf.st_mode;
        viktar.st_uid = statbuf.st_uid;
        viktar.st_gid = statbuf.st_gid;
        viktar.st_size = statbuf.st_size;

        viktar.st_atim.tv_sec = statbuf.st_atime;
        viktar.st_atim.tv_nsec = statbuf.st_atimensec;

        viktar.st_mtim.tv_sec = statbuf.st_mtime;
        viktar.st_mtim.tv_nsec = statbuf.st_mtimensec;

        viktar.st_ctim.tv_sec = statbuf.st_ctime;  
        viktar.st_ctim.tv_nsec = statbuf.st_ctimensec;  


        write(ofd, &viktar, sizeof(viktar_header_t));

        ifd = getFd(files[i], 1, 0, -1);
        if (verbose) fprintf(stderr, "> Input file descriptor: %d\n", ifd);

        bytesRead = read(ifd, data, statbuf.st_size * sizeof(char));
        if ((bytesWrite = write(ofd, data, bytesRead)) != bytesRead) {
            perror("Error occurred while writing to file");
        }
        if (verbose) fprintf(stderr, "> Bytes Read: %d\n> Bytes Write: %d\n", bytesRead, bytesWrite);

        free(data);
        close(ifd);
    }
    if (verbose) fprintf(stderr, "> Created viktar archive with %d files\n", numFiles);
    
    close(ofd);
    umask(old_mode);
}

void extractVik(char** files, int numFiles, char *fileName, int file) {
    viktar_header_t viktar = {0};
    int ifd = checkVik(fileName, file);
    int filesProcessed = 0;

    if (verbose) fprintf(stderr, "> Input file descriptor: %d\n", ifd);

    while(read(ifd, &viktar, sizeof(viktar_header_t)) > 0) {
        int proceed = 1;
        if (numFiles) proceed = checkFileName(files, numFiles, viktar.viktar_name);
        
        if (!proceed) lseek(ifd, viktar.st_size, SEEK_CUR);
        else {
            int bytesRead;
            int bytesWrite;
            struct utimbuf times;
            void *data = malloc(viktar.st_size*sizeof(char));
            int ofd = getFd(viktar.viktar_name, 1, 1, viktar.st_mode);

            if (verbose) fprintf(stderr, "> Output file descriptor: %d\n", ofd);
            
            bytesRead = read(ifd, data, viktar.st_size*sizeof(char));
            if ((bytesWrite = write(ofd, data, bytesRead)) != bytesRead) {
                perror("Error occurred while writing to file");
            }
            if (verbose) fprintf(stderr, "> Bytes Read: %d\n> Bytes Write: %d\n", bytesRead, bytesWrite);
            

            if (utime(viktar.viktar_name, NULL) == -1) perror("utime");
            
            times.actime = viktar.st_atim.tv_sec;
            times.modtime = viktar.st_mtim.tv_sec;

            if (utime(viktar.viktar_name, &times) == -1) perror("utime");

            free(data);
            close(ofd);
            filesProcessed++;
        }
    }
    close(ifd);
    if (verbose) fprintf(stderr, "Files processed: %d\n", filesProcessed);
}

int checkFileName(char** files, int numFiles, char *name) {
    if (verbose) fprintf(stderr, "> Checking if \"%s\" exists in archive\n", name);
    for (int i = 0; i < numFiles; i++) {
        if (strncmp(name, files[i], VIKTAR_MAX_FILE_NAME_LEN) == 0) {
            if (verbose) fprintf(stderr, "> \"%s\" found in viktar archive\n", name);
            return 1;
        }
    }
    
    if (verbose) fprintf(stderr, "> \"%s\" not found in archive\n", name);
    return 0;
}