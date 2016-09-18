/*************************************************************************
	> File Name: fcntl_access.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月18日 星期一 17时16分07秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>

//自定义的错误处理函数
void my_err(const char * err_string,int line)
{
    fprintf(stderr,"line:%d ",line);
    perror(err_string);
    exit(1);
}

int main(void)
{
    int ret;
    int access_mode;
    int fd;
    if((fd = open("./FILE/test_1",O_CREAT|O_RDWR|O_TRUNC,0700)) == -1) {
        my_err("open",__LINE__);
    }

    //设置文件打开方式
    if((ret == fcntl(fd,F_SETFL,O_APPEND)) < 0) {
        my_err("fcntl",__LINE__);//改变文件为追加方式打开
    }

    //获取文件的打开方式
    if((ret = fcntl(fd,F_GETFL,0)) < 0) {
        my_err("fcntl",__LINE__);
    }

    access_mode = ret & O_ACCMODE;//读写文件时，用来拉去flag的低２位．
    if(access_mode == O_RDONLY) {
        printf("test_1 access mode:read only"); //00
    } else if(access_mode == O_WRONLY) {
        printf("test_1 access mode:write only"); //01
    } else if(access_mode == O_RDWR) {
        printf("test_1 access mode:read and write"); //02
    }

    if(ret & O_APPEND) {
        printf(",append");
    }
    if(ret & O_NONBLOCK) {
        printf(",nonblock");//非阻塞的方式打开文件
    }
    if(ret & O_SYNC) {
        printf(",sync");//同步的方式打开文件
    }
    printf("\n");
    return 0;
}
