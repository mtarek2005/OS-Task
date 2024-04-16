#include <stdio.h>
#include <stddef.h>
#include <sys/stat.h>
#include <unistd.h>



void main(){
    //change permissions, from sys/stat.h
    //                r owner,w owner,r group,r other
    if(chmod("go.txt",S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))printf("error");

    //create file, from stdio.h
    FILE *f=fopen("ggg.txt","w");
    if(!f)printf("error");
    else fclose(f);

    //create dir, from sys/stat.h
    mkdir("Downloads2",S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

    //delete file or empty dir, from stdio.h
    remove("go.txt");
    remove("Downloads2");

    //change dir, from unistd.h
    //always call list and cwd again after
    chdir("Downloads");
    //list();
    //get current dir, from unistd.h
    char* path = getcwd(NULL,0);
    if(!path)printf("error");
    else printf(path);

    //symbolic link, from unistd.h
    //      original | shortcut
    symlink("ggg.txt","go.txt");
}