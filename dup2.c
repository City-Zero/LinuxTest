/*************************************************************************
	> File Name: dup2.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月18日 星期一 16时16分02秒
    >dup2()函数学习
    >dup2(现存文件描述符,可用文件描述符)
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>

int main(void)
{
    int fd,copyfd,cpfd;
    fd = open("./FILE/log.txt",O_RDWR);
    //指定文件描述符为1000;
    copyfd = dup2(fd,1000);
    //打印
    printf("fd:%d copyfd:%d\n",fd,copyfd);
    //指定文件描述符为1(标准输出)
    //若可用文件描述符已被系统打开,则会关闭他,故下面的printf()失效
    cpfd = dup2(fd,1);
    printf("cpfd:%d",cpfd);
    return 0;
}
