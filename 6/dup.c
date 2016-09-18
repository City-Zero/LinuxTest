/*************************************************************************
	> File Name: dup.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月18日 星期一 15时37分48秒
    >dup()函数学习
 ************************************************************************/

#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
int main(void)
{
    int fd,copyfd;
    fd = open("./FILE/log.txt",O_RDWR|O_CREAT,0644);//新建一个文件,权限为-rw-r--r--
    copyfd = dup(fd);
    //打印出fd和copyfd的描述
    printf("fd:%d copyfd:%d\n",fd,copyfd);//0,1,2被标准输入,标准输出,标准错误输出占据了
    //打印fd与copyfd的偏移量,均为0
    printf("fd:%d copyfd:%d\n",(int)lseek(fd,0,SEEK_CUR),(int)lseek(copyfd,0,SEEK_CUR));

    //给fd偏移量加3,查看copyfd的偏移量
    lseek(fd,3,SEEK_SET);
    printf("fd:%d copyfd:%d\n",(int)lseek(fd,0,SEEK_CUR),(int)lseek(copyfd,0,SEEK_CUR));
    char buf1[] = "hello ";
    char buf2[] = "world!";
    lseek(fd,0,SEEK_SET);
    lseek(copyfd,0,SEEK_SET);
    //往fd中写入数据
    if(write(fd,buf1,6) != 6) printf("write error!");

    //打印偏移量
    printf("fd:%d copyfd:%d\n",(int)lseek(fd,0,SEEK_CUR),(int)lseek(copyfd,0,SEEK_CUR));
    //往copyfd中写数据
    if(write(copyfd,buf2,6) != 6) printf("write error!");
    close(fd);
    return 0;
    //之后可以查看log.txt,有hello world!字符串了
}
