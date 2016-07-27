/*************************************************************************
	> File Name: new_sh.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月26日 星期二 09时39分17秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>

int main(int argc,char **argv,char **env)
{
    printf("uid = %d,euid = %d",getuid(),geteuid());
    sleep(3);
    execve("/bin/sh",argv,env);
    setuid(getuid());
    printf("uid = %d,euid = %d",getuid(),geteuid());
    sleep(3);
    return 0;
}
