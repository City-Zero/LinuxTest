/*************************************************************************
	> File Name: mv_test.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月21日 星期四 10时56分59秒
 ************************************************************************/

#include<stdio.h>
#include<errno.h>
#include<dirent.h>
#include<time.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/types.h>
#include<pwd.h>
#include<grp.h>
#include<stdlib.h>
#include<string.h>

int main()
{
	rename("./FILE/1","./1");
	perror("rename:");
	return 0;
}

