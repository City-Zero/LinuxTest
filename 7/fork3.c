/*************************************************************************
	> File Name: fork3.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月21日 星期四 17时40分13秒
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
		while(1) {
        printf("I'm a child process,my pid is %d,my parent pid id %d\n",getpid(),getppid());
		sleep(2);
      	}
    case -1:
        perror("Process creation failed\n");
        break;
    default:
        printf("I'm a Parent process,my pid id %d\n",getpid());
		exit(0);
    }
    return 0;
}
