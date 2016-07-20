/*************************************************************************
	> File Name: ls_R.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月20日 星期三 16时37分09秒
 ************************************************************************/

#include<stdio.h>
#include<dirent.h>
#include<time.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/types.h>
#include<pwd.h>
#include<grp.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#define MAX 256

void my_err(char *err_string, int line)
{
    fprintf(stderr, "line:%d", line);
    perror(err_string);
    exit(0);
}

void display_R(char *path)
{
    chdir(path);
    DIR *dir;
    struct stat buf;
    struct dirent *ptr;
    char filename[MAX];
    memset(filename, 0, MAX);
    strcpy(filename, path);
    dir = opendir(path);
    while((ptr = readdir(dir)) !=  NULL){
        printf("%s \n", ptr->d_name);
        if(lstat(ptr->d_name, &buf) == -1)
            my_err("lstat", __LINE__);
        if(S_ISDIR(buf.st_mode)){　　　　　　　　　　　　　　　　　//判断是否目录
            if(filename[strlen(path) - 1] != '/')             //判断目录最后一个字是不是‘/’，防止多加‘/’，目录判断出错。
                filename[strlen(path)] = '/';
            if(strcmp(ptr->d_name, ".") == 0)                 
                continue;                                      
           if(strcmp(ptr->d_name, "..") == 0)

                continue;　　　　　　　　　　　　　　　　　　　　
            filename[strlen(filename)] = '\0';　　　　　　　　　
            strcat(filename, ptr->d_name);
            printf("%s\n", filename);
            display_R(filename);                 　　　　　　　　　　//递归       
            chdir(path);                                        //函数执行完后返回，工作目录已改变。
            memset(filename, 0, MAX);　　　　　　　　　　　　　　　　//目录清空
            strcpy(filename, path);
        } 
    }
    closedir(dir);
    return;
}


int main(int argc, char **argv)
{
    display_R(argv[1]);
}
