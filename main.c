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
#include "utils.c"



void main(){
    // setup files at beginning 
    FILE *go=fopen("go.txt","w");
    if(!go)printf("error in create file: %s\n",strerror(errno));
    else fclose(go);
    mkdir("Downloads",S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
    remove("Downloads/go.txt");
    remove("Downloads/Downloads");
    
    // list current dir ".", defined in utils.h
    list_print(".");

    //change permissions, from sys/stat.h
    //                r owner,w owner,r group,r other
    if(chmod("go.txt",S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))printf("error in chmod: %s\n",strerror(errno));

    //create file, from stdio.h
    FILE *f=fopen("ggg.txt","w");
    if(!f)printf("error in create file: %s\n");
    else fclose(f);

    //create dir, from sys/stat.h
    if(mkdir("Downloads2",S_IRWXU|S_IRWXG|S_IROTH))printf("error in mkdir: %s\n",strerror(errno));

    //delete file or empty dir, from stdio.h
    if(remove("go.txt"))printf("error in remove file: %s\n",strerror(errno));
    if(remove("Downloads2"))printf("error in remove dir: %s\n",strerror(errno));

    //change dir, from unistd.h
    //always call list and cwd again after
    if(chdir("Downloads"))printf("error in chdir: %s\n",strerror(errno));
    printf("changed to ");
    //get current dir, from unistd.h
    char* path = getcwd(NULL,0);
    if(!path)printf("error in pwd: %s\n",strerror(errno));
    else printf("%s\n",path);
    //list again
    list_print(".");

    //symbolic link, from unistd.h
    //      original | shortcut
    if(symlink("../ggg.txt","go.txt"))printf("error in sym: %s\n",strerror(errno));
    if(symlink("../Downloads","Downloads"))printf("error in sym: %s\n",strerror(errno));
    list_print(".");
}