/*************************************************************************
	> File Name: processimage.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月22日 星期五 11时15分15秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>

int main(int argc,char **argv,char **environ)
{
    int i;

    printf("I am a process image!\n");
    printf("My pid is %d,parentdid is %d\n",getpid(),getppid());
    printf("uid = %d,gid = %d\n",getuid(),getgid());

    for(i=0;i < argc;i++)
        printf("argv[%d]:%s\n",i,argv[i]);
}
