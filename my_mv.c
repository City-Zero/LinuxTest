/*************************************************************************
	> File Name: my_mv.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月21日 星期四 09时23分11秒
 ************************************************************************/

#include<stdio.h>
#include<errno.h>
#include<dirent.h>
#include<time.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/types.h>
#include<pwd.h>
#include<grp.h>
#include<stdlib.h>
#include<string.h>

int main(int argc,char **argv)
{
    char path[50],file_name[20];
    int i,j=0;
    if(argc != 3) {
        printf("./my_mv <old file> <new file or path>");
        exit(0);
    }
    for(i=(strlen(argv[1])-1);i>=0;i--) {
        if(argv[1][i] == '/') {
            break;
        } else {
            file_name[j] = argv[1][i];
            j++;
        }
    }
    file_name[j] == '\0';
    char c;
    for(i=0;i<=((strlen(file_name)-1)/2);i++) {
        c=file_name[i];
        file_name[i] = file_name[strlen(file_name)-1-i];
        file_name[strlen(file_name)-1-i] = c;
    }
    if(rename(argv[1],argv[2]) == -1) {
        if(errno == 21) {
            strcpy(path,argv[2]);
            for(i=strlen(path)-1;path[i] != '/';i++) {
                path[i+1] = '/';
                path[i+2] = '\0';
                break;
            }
            strcat(path,file_name);
            rename(argv[1],path);
        }
    }
    if(errno != 21)
        perror("rename:");
    return 0;
}
