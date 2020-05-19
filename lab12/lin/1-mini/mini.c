/**
 * SO, 2016
 * Lab #12
 *
 * Task 01 - mini.c
 * Implementing a minimal comand line file utility
 */
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

#define MAX_LINE_SIZE 256

const char *delim = " \t\n";
char *prompt = "so-lab12";
struct stat statbuf;

#define TODO2
#define TODO3
#define TODO4
#define TODO5
#define TODO6
#define TODO7

int main(void) {
    char line[MAX_LINE_SIZE];
    char *cmd, *arg1, *arg2, *arg3;
    int ret; /* to be used for function calls return code */
    char type;
    char cwd[1024];
    DIR *mydir;
    struct dirent *myfile;
    struct stat mystat;
    char buf[512];

    while (1) {
        printf("<%s>", prompt);
        fflush(stdout);

        memset(line, 0, MAX_LINE_SIZE);

        if (fgets(line, sizeof(line), stdin) == NULL) exit(EXIT_SUCCESS);

        cmd = strtok(line, delim);
        if (!cmd) continue;
#ifdef DEBUG
        printf("Executing command: %s\n", cmd);
#endif
        if (strncmp(cmd, "quit", 4) == 0) break;
#ifdef TODO2
        /* implement list <device_node>
         * Output: <c|b> <major>:<minor>
         * e.g: list /dev/zero
         * Output: /dev/zero: <c> 1:5
         */
        if (strncmp(cmd, "list", 4) == 0) {
            arg1 = strtok(NULL, delim); /* device_node name */
            if (!arg1) continue;
            ret = stat(arg1, &statbuf);
            DIE(ret < 0, "stat failed");
            printf("Nume fisier = %s\n", cmd);
            switch (statbuf.st_mode & S_IFMT) {
                case S_IFBLK:
                    printf("block device\n");
                    type = 'b';
                    break;
                case S_IFCHR:
                    printf("character device\n");
                    type = 'c';
                    break;
                case S_IFDIR:
                    printf("directory\n");
                    type = 'd';
                    break;
                case S_IFIFO:
                    printf("FIFO/pipe\n");
                    type = 'f';
                    break;
                case S_IFLNK:
                    printf("symlink\n");
                    type = 's';
                    break;
                case S_IFREG:
                    printf("regular file\n");
                    type = 'r';
                    break;
                case S_IFSOCK:
                    printf("socket\n");
                    type = 's';
                    break;
                default:
                    printf("unknown?\n");
                    type = 'e';
                    break;
            }
            printf("%s: <%c> %lx:%lx\n", arg1, type,
                   (long)major(statbuf.st_dev), (long)minor(statbuf.st_dev));
        }
#endif

#ifdef TODO3
        /* implement mount source target fs_type
         * e.g: mount /dev/sda1 /mnt/disk2 ext3
         */
        if (strncmp(cmd, "mount", 5) == 0) {
            arg1 = strtok(NULL, delim); /* source */
            arg2 = strtok(NULL, delim); /* target */
            arg3 = strtok(NULL, delim); /* fs_type (e.g: ext2) */
            mount(arg1, arg2, arg3, 0, NULL);
        }
        if (strncmp(cmd, "umount", 6) == 0) {
            /* implement umount */
            arg1 = strtok(NULL, delim); /* target */
            umount(arg1);
        }
#endif

#ifdef TODO4
        /* implement symlink oldpath newpath
         * e.g: symlink a.txt b.txt
         */
        if (strncmp(cmd, "symlink", 7) == 0) {
            arg1 = strtok(NULL, delim); /* oldpath */
            arg2 = strtok(NULL, delim); /* newpath */
            symlink(arg1, arg2);
        }
        if (strncmp(cmd, "unlink", 6) == 0) {
            /* implement unlink */
            arg1 = strtok(NULL, delim); /* pathname */
            unlink(arg1);
        }
#endif

#ifdef TODO5
        /* implement mkdir pathname mode
         * e.g: mkdir food 0777
         */
        if (strncmp(cmd, "mkdir", 5) == 0) {
            arg1 = strtok(NULL, delim); /* pathname */
            mkdir(arg1, 0755);
        }
        if (strncmp(cmd, "rmdir", 5) == 0) {
            /* implement rmdir pathname */
            arg1 = strtok(NULL, delim); /* pathname */
            rmdir(arg1);
        }
#endif

#ifdef TODO6
        /* implement ls dirname
         * e.g: ls ..
         */
        if (strncmp(cmd, "ls", 2) == 0) {
            /* recursively print files starting with arg1 */

            arg1 = strtok(NULL, delim);
            if (arg1 == NULL) {
                arg1 = malloc(sizeof(char) * 2);
                strcpy(arg1, ".");
            }
            mydir = opendir(arg1);
            while ((myfile = readdir(mydir)) != NULL) {
                sprintf(buf, "%s/%s", arg1, myfile->d_name);
                stat(buf, &mystat);
                printf("%zu", mystat.st_size);
                printf(" %s\n", myfile->d_name);
            }
            closedir(mydir);
            free(arg1);
        }
#endif

#ifdef TODO7
        if (strncmp(cmd, "chdir", 5) == 0) {
            /* implement chdir <dir>
             * e.g: chdir bar
             */
            arg1 = strtok(NULL, delim); /* pathname */
            chdir(arg1);
            getcwd(cwd, sizeof(cwd));
            printf("Current directory: %s\n", cwd);
        }

        if (strncmp(cmd, "pwd", 3) == 0) {
            /* implement pwdir
             * e.g: pwd
             */
            /* print workding directory */
            getcwd(cwd, sizeof(cwd));
            printf("%s\n", cwd);
        }
#endif
    }

    return 0;
}
