#include <stdio.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

void list_print(char* path){
    DIR *d;
    //one file or dir from dir, from dirent.h
    struct dirent *dir;
    //open dir, from dirent.h
    d = opendir(path);
    if (d) {
        //loop over entries from readdir, from dirent.h
        while ((dir = readdir(d)) != NULL) {
            printf("%s\n", dir->d_name);
        }
        //close dir, from dirent.h
        closedir(d);
    }
    else{
        printf("error in list dir\n");
    }
}

void main(){
    // setup files at beginning 
    FILE *go=fopen("go.txt","w");
    if(!go)printf("error in create file\n");
    else fclose(go);
    mkdir("Downloads",S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    remove("Downloads/go.txt");

    // list current dir ".", defined above
    list_print(".");

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
    printf("changed to ");
    //get current dir, from unistd.h
    char* path = getcwd(NULL,0);
    if(!path)printf("error in pwd\n");
    else printf("%s\n",path);
    //list again
    list_print(".");

    //symbolic link, from unistd.h
    //      original | shortcut
    if(symlink("../ggg.txt","go.txt"))printf("error in sym\n");
}