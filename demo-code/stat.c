#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <time.h>

void main(){
    struct stat statbuf;
    stat("main.c",&statbuf);
    printf("%lu %lu %x %lu %u %u %lu %ld %ld %ld %ld %ld %ld y\n",statbuf.st_dev,statbuf.st_ino,statbuf.st_mode,statbuf.st_nlink,statbuf.st_uid,statbuf.st_gid,statbuf.st_rdev,statbuf.st_size,statbuf.st_blksize,statbuf.st_blocks,statbuf.st_atim.tv_sec,statbuf.st_mtim.tv_sec,statbuf.st_ctim.tv_sec);
    system("stat -Lc '%d %i %f %h %u %g %r %s %o %b %X %Y %Z' main.c");
    FILE* st=popen("stat -Lc '%d %i %f %h %u %g %r %s %o %b %X %Y %Z' main.c","r");
    fscanf(st,"%lu %lu %x %lu %u %u %lu %ld %ld %ld %ld %ld %ld",&statbuf.st_dev,&statbuf.st_ino,&statbuf.st_mode,&statbuf.st_nlink,&statbuf.st_uid,&statbuf.st_gid,&statbuf.st_rdev,&statbuf.st_size,&statbuf.st_blksize,&statbuf.st_blocks,&statbuf.st_atim.tv_sec,&statbuf.st_mtim.tv_sec,&statbuf.st_ctim.tv_sec);
    printf("%lu %lu %x %lu %u %u %lu %ld %ld %ld %ld %ld %ld z\n",statbuf.st_dev,statbuf.st_ino,statbuf.st_mode,statbuf.st_nlink,statbuf.st_uid,statbuf.st_gid,statbuf.st_rdev,statbuf.st_size,statbuf.st_blksize,statbuf.st_blocks,statbuf.st_atim.tv_sec,statbuf.st_mtim.tv_sec,statbuf.st_ctim.tv_sec);
    pclose(st);
}