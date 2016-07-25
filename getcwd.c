/*************************************************************************
	> File Name: getcwd.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月23日 星期六 09时12分06秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>

int main(void)
{
    char *path = NULL;
    while(1) {
        path = getcwd(NULL,0);
        puts(path);
        sleep(1);
        free(path);
    }
    return 0;
}
