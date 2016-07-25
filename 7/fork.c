/*************************************************************************
	> File Name: fork.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月21日 星期四 16时46分15秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
int main(void)
{
    pid_t pid;

    printf("Process Creation Study\n");
    pid = fork();
    switch(pid) {
    case 0:
        printf("Child process is running,CurPid is %d,ParentPid is %d\n",pid,getppid());
        break;
    case -1:
        perror("Process creation failed\n");
        break;
    default:
        printf("Parent process is running,ChildPid is %d,ParentPid is %d\n",pid,getpid());
        break;
    }
    exit(0);
}
