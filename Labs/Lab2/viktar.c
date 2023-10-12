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
#include <pwd.h>
#include <grp.h>
#include <time.h>

#include "viktar.h"

void printHelp(void);
int printContents(char*, int, int, int);
void strmode(mode_t, char*);
char getType (mode_t mode);
int getFd(char*, int, int);
int checkVik(char*, int);
void parseFiles(int, char**, char***);
void createVik(char**, int, char*, int);

int main(int argc, char *argv[]) {
    char filename[VIKTAR_MAX_FILE_NAME_LEN];
    int verbose = 0;
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
                fprintf(stderr, "verbose level: %d\n", verbose);
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
        if (!fileIn) fprintf(stderr, "archive from stdin\n");

        ifd = checkVik(filename, fileIn);
        
        ret = printContents(filename, longTOC, ifd, fileIn);
        if (ret == -1) fprintf(stderr, "some sorta error");
    }

    if (createMode || extractMode) {
        int total = argc - 2;
        char **files;
        int offset = 0;
        if (verbose) total--;
        offset = argc - total;

        files = calloc(total, sizeof(char*));
        for (int i = 0; i < total; i++) {
            files[i] = calloc(VIKTAR_MAX_FILE_NAME_LEN, sizeof(char));
            strcpy(files[i], argv[i + offset]);
            // printf("%s\n", files[i]);
        }

        //work here
        if (createMode) {
            createVik(files, total, filename, fileIn);
        }
        else {//extract mode

        }

        for (int i = 0; i < total; i++) free(files[i]);
        free(files);
    }

    return EXIT_SUCCESS;
}

void printHelp(void) {
    fprintf(stderr, "help text\n");
    fprintf(stderr, "\t./viktar\n");
    fprintf(stderr, "\t\t-x\t\textract file/files from archive\n");;
    fprintf(stderr, "\t\t-c\t\tcreate an archive file\n");;
    fprintf(stderr, "\t\t-t\t\tdisplay a short table of contents of the archive file\n");;
    fprintf(stderr, "\t\t-T\t\tdisplay a long table of contents of the archive file\n");;
    fprintf(stderr, "\t\tOnly one of xctT can be specified\n");
    fprintf(stderr, "\t\t-f filename\tuse filename as the archive file\n");
    fprintf(stderr, "\t\t-v\t\tgive verbose diagnostic messages\n");
    fprintf(stderr, "\t\t-h\t\tdisplay this AMAZING help message\n");
}

int printContents(char *fileName, int longTOC, int inIfd, int file) {
    
    viktar_header_t viktar = {0};
    int ifd = inIfd;
    
    if (!file) strcpy(fileName, "stdin");
    fprintf(stderr, "Contents of viktar file: \"%s\"\n", fileName);
    while(read(ifd, &viktar, sizeof(viktar_header_t)) > 0) {
        printf("\tfile name: %s\n", viktar.viktar_name);
        if (longTOC) {
            char mode[10];
            char fileType = getType(viktar.st_mode);
            struct passwd *pwd = getpwuid(viktar.st_uid);
            struct group *grp = getgrgid(viktar.st_gid); 
            char timeStr[100];
            strmode(viktar.st_mode, mode);

            printf("\t\tmode:\t%c%s\n", fileType, mode);
            printf("\t\tuser:\t%s\n", (pwd != NULL) ? pwd->pw_name : "unknown");
            printf("\t\tgroup:\t%s\n", (grp != NULL) ? grp -> gr_name : "unknown");
            printf("\t\tsize:\t%ld\n", viktar.st_size);
            
            strftime(timeStr, 100, "%Y-%m-%d %H:%M:%S %Z", localtime(&viktar.st_mtim.tv_sec));
            printf("\t\tmtime:\t%s\n", timeStr);

            strftime(timeStr, 100, "%Y-%m-%d %H:%M:%S %Z", localtime(&viktar.st_atim.tv_sec));
            printf("\t\tatime:\t%s\n", timeStr);

            lseek(ifd, viktar.st_size, SEEK_CUR);
        }
    }
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

int getFd(char* fileName, int file, int output) {
    int fd = -1;
    
    if (file) {
        if (output) fd = open(fileName, O_WRONLY | O_CREAT);
        else fd = open(fileName, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "failed to open input archive file\"%s\"\n", fileName);
            fprintf(stderr, "exiting...\n");
            exit(EXIT_FAILURE);
        }
    }
    else {
        fd = dup(0);
    }

    return fd;
}

int checkVik(char *fileName, int file) {
    char temp[10] = {0};
    int ifd = -1; 
    
    ifd = getFd(fileName, file, 0);;

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
    int ofd = getFd(fileName, file, 1);
    // for (int i = 0; i < numFiles; i++) if (!strcmp(files[i], fileName)) printf("yee\n");
    write(ofd, VIKTAR_FILE, sizeof(VIKTAR_FILE));
    for (int i = 0; i < numFiles; i++) {
        viktar_header_t viktar = {0};
        void* data;
        int ifd = -1;
        struct stat statbuf;
        int ret = stat(files[i], &statbuf);
        if (ret == -1) {
            fprintf(stderr, "\tfailed to stat file\"%s\"\n", files[i]);
            fprintf(stderr, "\texiting...\n");
            exit(EXIT_FAILURE);
        }

        strcpy(viktar.viktar_name, files[i]);
        viktar.st_mode = statbuf.st_mode;
        viktar.st_uid = statbuf.st_uid;
        viktar.st_gid = statbuf.st_gid;
        viktar.st_size = statbuf.st_size;
        viktar.st_atim.tv_sec = statbuf.st_atime;
        viktar.st_mtim.tv_sec = statbuf.st_mtime;
        viktar.st_ctim.tv_sec = statbuf.st_ctime;

        write(ofd, &viktar, sizeof(viktar));

        ifd = getFd(files[i], 1, 0);
        read(ifd, &data, statbuf.st_size);
        write(ofd, data, statbuf.st_size);

        close(ifd);
    }
    
    close(ofd);
}