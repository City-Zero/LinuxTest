/*************************************************************************
	> File Name: studyuid.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月23日 星期六 14时37分23秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>

extern int errno;

int main()
{
    int fd;
    printf("uid study:\n");
    printf("Process's uid = %d,euid = %d\n",getuid(),geteuid());
    
    //strerror 函数获取指定错误码提示信息
    if((fd = open("test.c",O_RDWR)) == -1) {
        printf("Open failure,errno is %d :%s \n",errno,strerror(errno));
        exit(1);
    } else {
        printf("Open successfully!\n");
    }
    setuid(getuid());
    printf("Process's uid = %d,euid = %d\n",getuid(),geteuid());
    close(fd);
    exit(0);
}
