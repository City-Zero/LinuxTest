/*************************************************************************
	> File Name: truncate.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月19日 星期二 14时30分59秒
    >change file size;argument one:file name,argument two:size(bytes).
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>

int main(int argc,char * argv[])
{
    if(argc != 3) {
        printf("argument errror!");
        exit(0);
    }

    if(truncate(argv[1],atoi(argv[2])) == 0) {
        printf("change size success!\n");
    } else {
        printf("change size fail!\n");
    }

    return 0;
}
