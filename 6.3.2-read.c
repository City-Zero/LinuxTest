/*************************************************************************
	> File Name: 6.1.2-read.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月18日 星期一 10时22分12秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>

//自定义错误预处理函数
void my_err(const char * err_string,int line)
{
    fprintf(stderr,"line:%d",line);//stderr 标准错误输出设备,使用方法:fprintf(stderr,"error message"),只会输出到屏幕,不会输出到文件
    perror(err_string);
    exit(1);
}

//自定义文件读取函数
int my_read(int fd)
{
    int len;
    int ret;
    int i;
    char read_buf[64];

    //获取文件长度并保持文件指针在开头
    if(lseek(fd,0,SEEK_END) == -1) {
        my_err("lseek",__LINE__);
    }

    if(len = lseek(fd, 0 , SEEK_CUR) == -1) {
        my_err("lseek",__LINE__);
    }

    if(lseek(fd,0,SEEK_SET) == -1) {
        my_err("lseek",__LINE__);
    }

    printf("len:%d\n",len);

    //读数据
    if((ret = read(fd,read_buf,len)) < 0) {
        my_err("read",__LINE__);
    }

    //打印数据
    for(i = 0;i < len;i++) {
        printf("%c",read_buf[i]);
    }
    printf("\n");

    return ret;
}

int main(void)
{
    int fd;
    char write_buf[32] = "Hello Worle!";
    //创建文件
    //if(fd = cerat("./FILE/test1.txt",S_IRWXU))
    if((fd = open("./FILE/test1.txt",O_CREAT|O_TRUNC|S_IRWXU)) == -1) {
        my_err("open",__LINE__);
    }
    else printf("creat file success\n");

    //写数据
    if (write(fd,write_buf,strlen(write_buf)) != strlen(write_buf))    {
        my_err("write",__LINE__);
    }

    my_read(fd);

    //--------------------
    printf("----------------\n");
    if(lseek(fd,10,SEEK_END) == -1) {
        my_err("lseek",__LINE__);
    }
    if(write(fd,write_buf,strlen(write_buf)) != strlen(write_buf)) {
        my_err("write",__LINE__);
    }
    my_read(fd);

    close(fd);
    return 0;
}
