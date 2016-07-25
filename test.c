/*************************************************************************
	> File Name: test.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月22日 星期五 21时36分08秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<fcntl.h>
#include<ctype.h>
#include<stdlib.h>
#include<errno.h>
#include<unistd.h>
int main()
{
    int i=0;
    char a[20];
    chdir("./in/");
    while(1) {
        sprintf(a,"%d",i);
        if(creat(a,O_CREAT) == -1) {
            perror("creat:");
            break;
        }
        i++;
    }
}
