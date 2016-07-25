/*************************************************************************
	> File Name: fork2.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月21日 星期四 17时13分08秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
int main(void)
{
    pid_t pid;
	int k;
	char *msg;

    printf("Process Creation Study\n");
    pid = fork();
    switch(pid) {
    case 0:
        msg = "Child process is running";
		k=3;
        break;
    case -1:
        perror("Process creation failed\n");
        break;
    default:
        msg = "Parent process is running";
		k=5;
        break;
    }
	while(k>0) {
	puts(msg);
	sleep(1);
	k--;
	}
    exit(0);
}
