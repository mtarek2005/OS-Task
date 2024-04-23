#ifndef UTILS_COMMON
#define UTILS_COMMON

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "str-utils.c"

int mkfile(const char* path);
int mkdir_default(const char* path);
typedef struct mode_bool
{
    bool ur,uw,ux,gr,gw,gx,or,ow,ox,su,sg,sv;
} mode_struct;
struct mode_bool mode_to_struct(mode_t mode){
    return (struct mode_bool){mode&S_IRUSR,mode&S_IWUSR,mode&S_IXUSR,mode&S_IRGRP,mode&S_IWGRP,mode&S_IXGRP,mode&S_IROTH,mode&S_IWOTH,mode&S_IXOTH,mode&S_ISUID,mode&S_ISGID,mode&S_ISVTX};
}
mode_t struct_to_mode(struct mode_bool mode){
    mode_t m=0;
    m|=mode.ur?S_IRUSR:0;
    m|=mode.uw?S_IWUSR:0;
    m|=mode.ux?S_IXUSR:0;
    m|=mode.gr?S_IRGRP:0;
    m|=mode.gw?S_IWGRP:0;
    m|=mode.gx?S_IXGRP:0;
    m|=mode.or?S_IROTH:0;
    m|=mode.ow?S_IWOTH:0;
    m|=mode.ox?S_IXOTH:0;
    m|=mode.su?S_ISUID:0;
    m|=mode.sg?S_ISGID:0;
    m|=mode.sv?S_ISVTX:0;
    return m;
}
struct dent_agnostic
{
    struct stat statbuf;
    void *dir;
    bool is_dir,is_lnk;
    char* name;
    char* path;
    void* entry;
};

#endif