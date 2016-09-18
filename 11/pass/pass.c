/*************************************************************************
	> File Name: pass.c
	> Author: 
	> Mail: 
	> Created Time: 2016年08月05日 星期五 10时24分34秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<string.h>
int main(void)
{
    char pa[20],pp[20];
    memcpy(pa,getpass("passwd:"),20);
	memcpy(pp,getpass("dasd:"),20);
	puts(pa);
    puts(pp);
    return 0;
}
