/*************************************************************************
	> File Name: mynice.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月25日 星期一 15时19分35秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/resource.h>
#include<sys/wait.h>
#include<stdlib.h>

int main(void)
{
    pid_t pid;
    int stat_val = 0;
    int oldpri,newpri;

    printf("nice study\n");

    pid = fork();
    switch(pid) {
    case 0:
        printf("Child is running,CurPid is %d,ParentPid is %d\n",pid,getppid());
        oldpri = getpriority(PRIO_PROCESS,0);
        printf("Old priority = %d\n",oldpri);

        newpri = nice(2);
        printf("New priority= %d\n",newpri);

        exit(0);
    case -1:
        perror("Process ceration failed\n");
        break;
    defaule:
        printf("Parent is running,ChildPid is %d,ParentPid is %d\n",pid,getpid());
        break;
    }

    wait(&stat_val);
    exit(0);
}
