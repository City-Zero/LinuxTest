/*************************************************************************
	> File Name: my_chmod.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月19日 星期二 11时27分27秒
 ************************************************************************/

#include<stdio.h>
#include<time.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/types.h>
#include<errno.h>
#include<stdlib.h>

int main(int argc,char *argv[])
{
    struct stat buf;//文件属性结构体

    //检查参数
    if(argc !=2) {
        printf("Usage:my_stat <filename>\n");
        exit(0);
    }

    //获取文件属性
    if(stat(argv[1],&buf) == -1) {
        perror("stat:");
        exit(1);
    }

    //打印文件属性
    printf("device is:%ld\n",buf.st_dev);
    printf("inode is:%ld\n",buf.st_ino);
    printf("mode id:%o\n",buf.st_mode);
    printf("number of hard link is: %ld\n",buf.st_nlink);
    printf("UID:%d\n",buf.st_uid);
    printf("GID:%d\n",buf.st_gid);
    printf("device type: %ld\n",buf.st_rdev);

    printf("total size: %ld bytes\n",buf.st_size);
    printf("blocksize for filesystem I/O is: %ld\n",buf.st_blksize);
    printf("number if blocks allicated is: %ld\n",buf.st_blocks);

    printf("time of last access is: %s",ctime(&buf.st_atime));
    printf("time of last modification is :%s",ctime(&buf.st_mtime));
    printf("time of last change is : %s",ctime(&buf.st_ctime));

    return 0;
}
