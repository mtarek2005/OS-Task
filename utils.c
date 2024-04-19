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
#include <string.h>
#include <errno.h>
#include <limits.h>

int print_errno(int err,char* desc){
    if(err) printf("error at %s: %s\n",desc,strerror(errno));
    return err;
}
void* print_errno_null(void* err,char* desc){
    if(!err) printf("error at %s: %s\n",desc,strerror(errno));
    return err;
}
char* combine_path(char* path,char* name){
    size_t path_len = strlen(path);
    size_t full_len = path_len+strlen(name)+2;
    char* str = malloc(full_len);
    strcpy(str,path);
    str[path_len]='/';
    str[path_len+1]=0;
    strcat(str,name);
    return str;
}
char* combine_arg_unafe(char* path,char* name){
    size_t path_len = strlen(path);
    size_t full_len = path_len+strlen(name)+2;
    char* str = malloc(full_len);
    strcpy(str,path);
    str[path_len]=' ';
    str[path_len+1]=0;
    strcat(str,name);
    return str;
}
char* combine_arg(char* path,char* name){
    size_t path_len = strlen(path);
    size_t name_len = strlen(name);
    size_t path_len_e = path_len;
    size_t name_len_e = name_len;
    for(int i=0;i<path_len;i++){
        if(path[i]=='"')path_len_e++;
    }
    for(int i=0;i<name_len;i++){
        if(name[i]=='"')name_len_e++;
    }
    char* path_e=malloc(path_len_e+1);
    char* name_e=malloc(name_len_e+1);
    for(int i=0,j=0;i<path_len&&j<path_len_e;i++,j++){
        if(path[i]=='"'){
            path_e[j]='\\';
            j++;
        }
        path_e[j]=path[i];
    }
    path_e[path_len_e]=0;
    for(int i=0,j=0;i<name_len&&j<name_len_e;i++,j++){
        if(name[i]=='"'){
            name_e[j]='\\';
            j++;
        }
        name_e[j]=name[i];
    }
    name_e[name_len_e]=0;
    size_t full_len = path_len_e+name_len_e+1+4+1;
    char* str = malloc(full_len);
    str[0]='"';
    str[1]=0;
    strcat(str,path_e);
    str[path_len_e+1]='"';
    str[path_len_e+2]=' ';
    str[path_len_e+3]='"';
    str[path_len_e+4]=0;
    strcat(str,name_e);
    str[full_len-2]='"';
    str[full_len-1]=0;
    free(path_e);
    free(name_e);
    return str;
}
char* combine_arg_2(char* path,char* name){
    size_t path_len = strlen(path);
    size_t name_len = strlen(name);
    size_t name_len_e = name_len;
    for(int i=0;i<name_len;i++){
        if(name[i]=='"')name_len_e++;
    }
    char* name_e=malloc(name_len_e+1);
    for(int i=0,j=0;i<name_len&&j<name_len_e;i++,j++){
        if(name[i]=='"'){
            name_e[j]='\\';
            j++;
        }
        name_e[j]=name[i];
    }
    name_e[name_len_e]=0;
    size_t full_len = path_len+name_len_e+1+2+1;
    char* str = malloc(full_len);
    strcpy(str,path);
    str[path_len+0]=' ';
    str[path_len+1]='"';
    str[path_len+2]=0;
    strcat(str,name_e);
    str[full_len-2]='"';
    str[full_len-1]=0;
    free(path);
    free(name_e);
    return str;
}

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

#endif