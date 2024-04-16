#include <stdio.h>
#include <stddef.h>
#include <sys/stat.h>
#include <unistd.h>



void main(){
    //change permissions, from sys/stat.h
    //                r owner,w owner,r group,r other
    if(chmod("go.txt",S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))printf("error in chmod\n");

    //create file, from stdio.h
    FILE *f=fopen("ggg.txt","w");
    if(!f)printf("error in create file\n");
    else fclose(f);

    //create dir, from sys/stat.h
    if(mkdir("Downloads2",S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))printf("error in mkdir\n");

    //delete file or empty dir, from stdio.h
    if(remove("go.txt"))printf("error in remove file\n");
    if(remove("Downloads2"))printf("error in remove dir\n");

    //change dir, from unistd.h
    //always call list and cwd again after
    if(chdir("Downloads"))printf("error in chdir\n");
    //list();
    //get current dir, from unistd.h
    char* path = getcwd(NULL,0);
    if(!path)printf("error in pwd\n");
    else printf("%s\n",path);

    //symbolic link, from unistd.h
    //      original | shortcut
    if(symlink("../ggg.txt","go.txt"))printf("error in sym\n");
}