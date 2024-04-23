#ifndef UTILS_NATIVE
#define UTILS_NATIVE

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include "str-utils.c"

int rm_rec(char* path){
    //directory object for opendir(), from dirent.h
    DIR *d;
    //one file or directory from inside the directory, from dirent.h
    struct dirent *dir;
    //open dir, from dirent.h
    d = opendir(path);
    if (d) {
        //loop over entries from readdir, from dirent.h
        while ((dir = readdir(d)) != NULL) {
            // check if it's not "." or ".."
            if(strcmp(dir->d_name,".")&&strcmp(dir->d_name,"..")){
                //check if it's a dir
                bool is_dir=dir->d_type==DT_DIR;;
                //delete if file
                if(!is_dir) {if(print_errno(unlinkat(dirfd(d),dir->d_name,0),"unlink")) return -1;}
                else {
                    char* dpath=combine_path(path,dir->d_name);
                    rm_rec(dpath);
                    free(dpath);
                }
            }
        }
        //close dir, from dirent.h
        print_errno(closedir(d),"close dir");
        return print_errno(remove(path),"close dir");
    }
    else{
        print_errno(-1,"open dir");
        return -1;
    }
}

void list_print(char* path){
    //directory object for opendir(), from dirent.h
    DIR *d;
    //one file or directory from inside the directory, from dirent.h
    struct dirent *dir;
    //open dir, from dirent.h
    d = opendir(path);
    if (d) {
        //loop over entries from readdir, from dirent.h
        while ((dir = readdir(d)) != NULL) {
            // check if it's not "." or ".."
            if(strcmp(dir->d_name,".")&&strcmp(dir->d_name,"..")){
                // get info of entry(including link target), from sys/stat.h
                struct stat statbuf;
                print_errno(fstatat(dirfd(d),dir->d_name,&statbuf,0),"fstatat");
                //check if it's a link
                bool is_lnk=dir->d_type==DT_LNK;
                //check if it's a dir
                bool is_dir=S_ISDIR(statbuf.st_mode);
                //print entry name (dir->d_name) and if it's a folder(is_dir) or link(is_lnk)
                printf("%s    %s %s\n", dir->d_name,is_dir?"dir":"file",is_lnk?"link":"");
            }
        }
        //close dir, from dirent.h
        print_errno(closedir(d),"close dir");
    }
    else{
        print_errno(1,"open dir");
    }
}

int mkfile(const char* path){
    FILE *f=fopen(path,"w");
    if(!f){
        printf("error in create file: %s\n");
        return -1;
    }
    else fclose(f);
    return 0;
}
int mkdir_default(const char* path){
    return mkdir(path,S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
}

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

// directory entry with extra data
struct dent_agostic
{
    struct stat statbuf;
    struct dirent *dir;
    bool is_dir,is_lnk;
    char* name;
    char* path;
    void* entry;
};
// fill array with directory contents
size_t list_inarr(char* path, struct dent_agostic elms[], size_t elms_max_len){
    //final size to be returned
    size_t elms_len=0;
    //directory object for opendir(), from dirent.h
    DIR *d;
    //one file or directory from inside the directory, from dirent.h
    struct dirent *dir;
    //open dir, from dirent.h
    d = opendir(path);
    int i=0;
    if (d) {
        //loop over entries from readdir, from dirent.h
        while ((dir = readdir(d)) != NULL && i<elms_max_len) {
            // check if it's not "." or ".."
            if(strcmp(dir->d_name,".")/*&&strcmp(dir->d_name,"..")*/){
                // get info of entry(including link target), from sys/stat.h
                struct stat statbuf;
                print_errno(fstatat(dirfd(d),dir->d_name,&statbuf,0),"fstatat(list_dir)");
                //check if it's a link
                bool is_lnk=dir->d_type==DT_LNK;
                //check if it's a dir
                bool is_dir=S_ISDIR(statbuf.st_mode);
                //add to array
                elms[i]=(struct dent_agostic){statbuf,dir,is_dir,is_lnk,strdup(dir->d_name),NULL,NULL};
                elms[i].path=combine_path(path,elms[i].name);
                i++;
            }
        }
        elms_len=i;
        //close dir, from dirent.h
        print_errno(closedir(d),"close dir");
    }
    else{
        print_errno(1,"open dir");
    }
    return elms_len;
}
//compare directory entries for qsort
static int cmpstringp(const void *p1, const void *p2) { 
    /* The actual arguments to this function are "pointers to 
        pointers to char", but strcmp(3) arguments are "pointers 
        to char", hence the following cast plus dereference. */ 
    return strcmp(((struct dent_agostic*) p1)->name, ((struct dent_agostic*) p2)->name); 
}

#endif