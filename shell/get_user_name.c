/*************************************************************************
	> File Name: get_user_name.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月29日 星期五 08时48分58秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>

int main()
{
    char re[40];
    memset(re,40,0);
    char name[10];
    char hostname[10];
    int fd1,fd2;
    system("whoami > /tmp/host_name");
    fd2 = open("/etc/hostname",O_RDONLY);
    read(fd2,hostname,10);
    close(fd2);
    int i;
    for(i=0;hostname[i]!='\n';i++) ;
    hostname[i] = '\0';
    puts(hostname);
    fd1= open("/tmp/host_name",O_RDONLY);
    read(fd1,name,10);
    close(fd2);
    remove("/tmp/hostname");
    for(i=0;name[i]!='\n';i++) ;
    name[i] = '\0';
	puts(name);
    strcpy(re,"\e[35;1m");
    strcat(re,name);
    strcat(re,"@");
    strcat(re,hostname);
    strcat(re,"\e[0m");
    if(strcmp(name,"root") == 0) {
    strcat(re," # ");
    } else {
    strcat(re," $ ");
    }
    puts(re);
}
