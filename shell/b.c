/*************************************************************************
	> File Name: b.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月26日 星期二 16时57分24秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main(void)
{
    char *arglist[3];

    arglist[0] = "ls";
    arglist[1] = "-l";
    arglist[2] = NULL;
    execvp("/bin/ls",arglist);
}
