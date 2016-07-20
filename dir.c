/*************************************************************************
	> File Name: dir.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月20日 星期三 08时10分35秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

int my_readdir(const char *path)
{
    DIR *dir;
    struct dirent *ptr;
    int name_len=0;
    int num=0;
    int i,j;
    char **f;
    char *temp;
    if((dir = opendir(path)) == NULL) {
        perror ("opendir");
        return -1;
    }
    while ((ptr = readdir(dir)) != NULL) {
        num++;
        if(strlen(ptr->d_name) > name_len) {
            name_len = strlen(ptr->d_name);
        }
    }
    closedir(dir);

    temp = (char*)malloc(sizeof(char)*name_len);
    dir = opendir(path);
    f = (char **)malloc(sizeof(char*)*num);
    for(i=0;i<num;i++) {
        f[i] = (char *)malloc(sizeof(char)*(name_len+1));
    }
    i=0;
    while ((ptr = readdir(dir)) != NULL) {
        strcpy(f[i],ptr->d_name);
        i++;
    }
    for(i=0;i<num-1;i++) {
        for(j=0;j<num-i-1;j++) {
            if(strcmp(f[j],f[j+1]) > 0) {
                strcpy(temp,f[j]);
                strcpy(f[j],f[j+1]);
                strcpy(f[j+1],temp);
            }
        }
    }
    char na[3];
    if(name_len > 9) {
        na[0] = (name_len/10) + 48;
        na[1] = (name_len%10) + 48;
        na[2] = '\0';
    }
    else {
        na[0] = name_len + 48;
        na[1] = '\0';
    }
    char a[10]="%-";
    strcat(a,na);
    char s[]="s ";
    strcat(a,s);
    for(i=0;i<num;i++) {
        printf(a,f[i]);
    }

    return 0;
}
int main(int argc,char **argv)
{
    if(argc == 1) {
        my_readdir("./");
    }
    else if(argc > 1) {
        int i;
        for(i=1;i<argc;i++) {
            printf("\033[0;34;1m%s\n\033[0m",argv[i]);
            my_readdir(argv[i]);
        }
    }

    return 0;
}
