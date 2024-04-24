#ifndef UTILS_SHELL
#define UTILS_SHELL

#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include "common-utils.h"

int rm_rec(const char* path){
    return print_errno_return(system(combine_arg_2(strdup("rm -rf"),path)),"rm dir");
}
int rm(const char* path){
    return print_errno_return(system(combine_arg_2(strdup("rm -f"),path)),"rm");
}
char* pwd(){
    FILE* p=popen("pwd","r");
    char* line=NULL;
    size_t line_sz=0,line_len;
    line_len=getline(&line,&line_sz,p);
    if(pclose(p)||line_len==-1) {
        free(line);
        return NULL;
    }
    else {
        line[line_len-1]=0;
        return line;
    }
}
int ln(const char *from, const char *to){
    char* cmd=strdup("ln -sf");
    cmd=combine_arg_2(cmd,from);
    cmd=combine_arg_2(cmd,to);
    return print_errno_return(system(cmd),"ln");
}
int stat_univ(const char *file, struct stat *buf){
    char* cmd=strdup("stat -Lc '%d %i %f %h %u %g %r %s %o %b %X %Y %Z'");
    cmd=combine_arg_2(cmd,file);
    FILE* st=popen(cmd,"r");
    fscanf(st,"%lu %lu %x %lu %u %u %lu %ld %ld %ld %ld %ld %ld",&(buf->st_dev),&(buf->st_ino),&(buf->st_mode),&(buf->st_nlink),&(buf->st_uid),&(buf->st_gid),&(buf->st_rdev),&(buf->st_size),&(buf->st_blksize),&(buf->st_blocks),&(buf->st_atim.tv_sec),&(buf->st_mtim.tv_sec),&(buf->st_ctim.tv_sec));
    buf->st_atim.tv_nsec=0;
    buf->st_ctim.tv_nsec=0;
    buf->st_mtim.tv_nsec=0;
    free(cmd);
    return pclose(st);
}
int lstat_univ(const char *file, struct stat *buf){
    char* cmd=strdup("stat -c '%d %i %f %h %u %g %r %s %o %b %X %Y %Z'");
    cmd=combine_arg_2(cmd,file);
    FILE* st=popen(cmd,"r");
    fscanf(st,"%lu %lu %x %lu %u %u %lu %ld %ld %ld %ld %ld %ld",&(buf->st_dev),&(buf->st_ino),&(buf->st_mode),&(buf->st_nlink),&(buf->st_uid),&(buf->st_gid),&(buf->st_rdev),&(buf->st_size),&(buf->st_blksize),&(buf->st_blocks),&(buf->st_atim.tv_sec),&(buf->st_mtim.tv_sec),&(buf->st_ctim.tv_sec));
    buf->st_atim.tv_nsec=0;
    buf->st_ctim.tv_nsec=0;
    buf->st_mtim.tv_nsec=0;
    free(cmd);
    return pclose(st);
}
int chmod_univ(const char *file, mode_t mode){
    mode_t mode_only=mode&07777;
    char str[20]={0};
    snprintf(str,20,"%o",mode_only);
    char* cmd=strdup("chmod");
    cmd=combine_arg(cmd,str);
    cmd=combine_arg_2(cmd,file);
    return print_errno_return(system(cmd),"chmod");
}
void list_print(char* path){
    print_errno_return(system(combine_arg_2(strdup("ls -a"),path)),"ls");
}

int mkfile(const char* path){
    return print_errno_return(system(combine_arg(strdup("touch"),path)),"touch");
}
int mkdir_default(const char* path){
    return print_errno_return(system(combine_arg_2(strdup("mkdir -p"),path)),"mkdir");
}



// directory entry with extra data
struct dent_char
{
    struct stat statbuf;
    char *dir;
    bool is_dir,is_lnk;
    char* name;
    char* path;
    void* entry;
};
// fill array with directory contents
size_t list_inarr(char* path, struct dent_agnostic elms_a[], size_t elms_max_len){
    struct dent_char* elms=(struct dent_char*)elms_a;
    //final size to be returned
    size_t elms_len=0;
    //directory object for opendir(), from dirent.h
    FILE *p;
    //one file or directory from inside the directory, from dirent.h
    char *dir=NULL;
    size_t dir_size=0;
    size_t dir_len;
    //open dir, from dirent.h
    p = popen(combine_arg_2(strdup("ls -a"),path),"r");
    int i=0;
    if (p) {
        //loop over entries from readdir, from dirent.h
        while ((dir_len = getline(&dir,&dir_size,p)) != -1 && i<elms_max_len) {
            dir[dir_len-1]=0;
            // check if it's not "." or ".."
            if(strcmp(dir,".")/*&&strcmp(dir->d_name,"..")*/){
                // get info of entry(including link target), from sys/stat.h
                struct stat statbuf;
                struct stat statbuf_og;
                char* full_path=combine_path(path,dir);
                print_errno_return(stat_univ(full_path,&statbuf),"stat(ls)");
                print_errno_return(lstat_univ(full_path,&statbuf_og),"lstat(ls)");
                //check if it's a link
                bool is_lnk=S_ISLNK(statbuf_og.st_mode);
                //check if it's a dir
                bool is_dir=S_ISDIR(statbuf.st_mode);
                //add to array
                elms[i]=(struct dent_char){statbuf,NULL,is_dir,is_lnk,strdup(dir),NULL,NULL};
                elms[i].path=combine_path(path,elms[i].name);
                free(full_path);
                i++;
            }
        }
        elms_len=i;
        //close dir, from dirent.h
        print_errno(pclose(p),"close ls");
    }
    else{
        print_errno_null(p,"open ls");
    }
    return elms_len;
}


#endif