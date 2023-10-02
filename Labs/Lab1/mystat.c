/* Omar Nassar
 * October 1, 2023
 * Portland State University CS333
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

char FileType (mode_t mode);
void strmode(mode_t mode, char * buf);

int main(int argc, char *argv[]) {

    if (argc < 2) return EXIT_FAILURE;

    for (int i = 1; i < argc; i++) {
        struct stat statbuf;
        int ret = lstat(argv[i], &statbuf);
        if (ret == -1) fprintf(stderr, "mystat: cannot access '%s': No such file or directory\n", argv[i]);
        // printf("%s, %d\n", argv[i], ret);
        else {
            char mode[10];
            int chmod;
            char fileType;
            char fileTypeStr[100];
            struct stat link;
            int uid = 0;
            int gid = 0;
            int sticky = 0;
            struct passwd *pwd;
            struct group *grp;
            char timeStr[100];
            time_t time;
            struct tm *timeStruct;
            
            fileType = FileType(statbuf.st_mode);
            if (fileType == '-') strcpy(fileTypeStr, "regular file");
            else if (fileType == 'd') strcpy(fileTypeStr, "directory");
            else if (fileType == 'p') strcpy(fileTypeStr, "FIFO/pipe");
            else if (fileType == 's') strcpy(fileTypeStr, "socket");
            else if (fileType == 'c') strcpy(fileTypeStr, "character device");
            else if (fileType == 'b') strcpy(fileTypeStr, "block device");
            else if (fileType == 'l') {
                char linkname[100];
                memset(linkname, 0, sizeof(char) * 100);

                readlink(argv[i], linkname, 100);

                strcpy(fileTypeStr, "Symbolic link ");


                if (stat(linkname, &link) == 0) {
                    strcat(fileTypeStr, "-> ");
                    strcat(fileTypeStr, linkname);
                }
                else strcat(fileTypeStr, "- with dangling destination");

            }
            else strcpy(fileTypeStr, "Unknown");

            printf("File: %s\n", argv[i]);
            printf("  File type:                %s\n", fileTypeStr);

            if (statbuf.st_mode & S_ISUID) uid = 1;
            if (statbuf.st_mode & S_ISGID) gid = 1;
            if (statbuf.st_mode & __S_ISVTX) sticky = 1;

            if (uid || gid || sticky) {
                printf("  Special bits set:         ");
                if (uid) printf("set-UID ");
                if (gid) printf("set-GID ");
                if (sticky) printf("sticky bits ");
                printf("\n");
            }
            
            printf("  Device ID number:         %lxh/%ldd\n", statbuf.st_dev, statbuf.st_dev);
            printf("  I-node number:            %ld\n", statbuf.st_ino);
            
            strmode(statbuf.st_mode, mode);
            chmod = statbuf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
            if (gid) mode[5] = 'S';
            printf("  Mode:                     %c%s        (%03o in octal)\n", fileType, mode, chmod);
            
            printf("  Link count:               %ld\n", statbuf.st_nlink);
            
            pwd = getpwuid(statbuf.st_uid);
            printf("  Owner Id:                 %s           (UID = %d)\n", (pwd != NULL) ? pwd -> pw_name : "unknown",  statbuf.st_uid);
            grp = getgrgid(statbuf.st_gid);
            printf("  Group Id:                 %s              (GID = %d)\n", (pwd != NULL) ? grp -> gr_name : "unknown",  statbuf.st_gid);

            printf("  Preferred I/O block size: %ld bytes\n", statbuf.st_blksize);
            printf("  File size:                %ld bytes\n", statbuf.st_size);
            printf("  Blocks allocated:         %ld\n", statbuf.st_blocks);
            
            time = statbuf.st_atime;
            timeStruct = localtime(&time);
            strftime(timeStr, 100, "%Y-%m-%d %H:%M:%S %z (%Z) %a", timeStruct);
            printf("  Last file access:         %s (local)\n", timeStr);

            time = statbuf.st_mtime;
            timeStruct = localtime(&time);
            strftime(timeStr, 100, "%Y-%m-%d %H:%M:%S %z (%Z) %a", timeStruct);
            printf("  Last file modification:   %s (local)\n", timeStr);

            time = statbuf.st_ctime;
            timeStruct = localtime(&time);
            strftime(timeStr, 100, "%Y-%m-%d %H:%M:%S %z (%Z) %a", timeStruct);
            printf("  Last status change:       %s (local)\n", timeStr);
        }
    }

    return EXIT_SUCCESS;
}

void strmode(mode_t mode, char * buf) {
  const char chars[] = "rwxrwxrwx";
  for (size_t i = 0; i < 9; i++) {
    buf[i] = (mode & (1 << (8-i))) ? chars[i] : '-';
  }
  buf[10] = '\0';
}

char FileType (mode_t mode) {
    switch (mode & __S_IFMT) {            //bitwise AND to determine file type
        case __S_IFSOCK:  return 's';     //socket
        case __S_IFLNK:   return 'l';     //symbolic link
        case __S_IFREG:   return '-';     //regular file
        case __S_IFBLK:   return 'b';     //block device
        case __S_IFDIR:   return 'd';     //directory
        case __S_IFCHR:   return 'c';     //char device
        case __S_IFIFO:   return 'p';     //pipe
        default: return '?';            //unknown
    }
}