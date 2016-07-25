/*************************************************************************
	> File Name: wait.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月22日 星期五 15时56分23秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>

int main(int argc,char **argv,char **envp)
{
    pid_t pid;
    char *msg;
    int k;
    int exit_code;

    printf("Study how to get exit code\n");
    pid = fork();

    switch(pid) {
    case 0:
        msg = "child process is running";
        k = 5;
        exit_code = 37;
        break;
    case -1:
        perror("process creation failed\n");
        exit(1);
    defaule:
        exit_code = 0;
        break;
    }

    //子父进程都会执行这段代码
    //子进程pid=0,父进程pid值为子进程的ID
    if(pid == 0) {//父进程等待子进程结束
        int stat_val;
        pid_t child_pid;
        child_pid = wait(&stat_val);
        printf("Child process has exited,pid = %d\n",child_pid);
        if(WIFEXITED(stat_val)) {
            printf("Child exited with code %d\n",WEXITSTATUS(stat_val));
        } else {
            printf("Child exited abnormall\n");
        }
    }
    else {
        while(k-- > 0) {
            puts("12345");
            sleep(1);
        }
    }
}
