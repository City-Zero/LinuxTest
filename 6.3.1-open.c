/*************************************************************************
	> File Name: 6.3.1-open.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月18日 星期一 09时46分03秒
 ************************************************************************/

#include<stdio.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<stdlib.h>

int main(void)
{
    int fd;
    if((fd = open("./FILE/test.txt",O_CREAT|O_EXCL,S_IRUSR|S_IWUSR)) == -1){
    //O_CREAT新建文件 O_EXCL:与O_CREAT一起使用,检查文件是否存在,不存在则创建,存在则打开失败
    //if((fd = creat("./FILE/test.txt",S_IRWXU)) == -1){
        perror("open");
        //printf("open:%s   with errno:%d\n",strerror(errno),errno);
        exit(1);
    }
    else printf("create file success\n");
    close(fd);
    return 0;

}
