/*************************************************************************
	> File Name: forkdemo2.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月26日 星期二 11时31分01秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main(void)
{
    printf("my pid is %d\n",getpid());

    fork();
    fork();
    fork();

    printf("my pid is %d\n",getpid());
}
