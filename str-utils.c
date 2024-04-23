#ifndef UTILS_STR
#define UTILS_STR

#include <string.h>
#include <errno.h>

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


#endif