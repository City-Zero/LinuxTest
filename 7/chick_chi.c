/*************************************************************************
	> File Name: chick_chi.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月22日 星期五 14时54分59秒
    >>>>>>利用wait(child_pid,NULL,WNOHANG)来不断判断子进程是否退出
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc,char **argv,char **envp)
{
    pid_t pc,pr;

    pc = fork();

    switch (pc) {
    case 0:
        puts("child process running!");
        sleep(50);
        exit(0);
    default:
        puts("parents process running!");
        while(1) {
            pr = waitpid(pc,NULL,WNOHANG);
            if(pr == 0) {
                puts("child process is running!");
            } else if(pr == -1) {
                puts("1:error");
            } else {
                printf("%d child process is stop!\n",pr);
                exit(0);
            }
            sleep(1);
        }
    case -1:
        puts("def:error");
    }
    return 0;
}
