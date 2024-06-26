#ifndef UTILS_NATIVE
#define UTILS_NATIVE

#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include "common-utils.h"

int rm_rec(const char* path){
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
int rm(const char* path){
    return remove(path);
}
char* pwd(){
    return getcwd(NULL,0);
}
int ln(const char *from, const char *to){
    return symlink(from,to);
}
int stat_univ(const char *file, struct stat *buf){
    return stat(file,buf);
}
int chmod_univ(const char *file, mode_t mode){
    return chmod(file,mode);
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



// fill array with directory contents
size_t list_inarr(char* path, struct dent_agnostic elms[], size_t elms_max_len){
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
                elms[i]=(struct dent_agnostic){statbuf,is_dir,is_lnk,strdup(dir->d_name),combine_path(path,dir->d_name),NULL};
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


#endif