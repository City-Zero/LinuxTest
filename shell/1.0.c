/*************************************************************************
	> File Name: 1.0.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月26日 星期二 14时12分56秒
    >>var 1.0
    >>可实现普通命令的键入(无>,<,|等的输入)
    >>输入exit退出程序
    >>命令执行是可按ctrl+c停止
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<dirent.h>
#include<signal.h>

#define normal          0//一般命令
#define out_redirect    1//输出重定向
#define in_redirect     2//输入重定向
#define have_pipd       3//命令中由管道
void goodbye(void);
void print_tishi(void);
void get_input(char *shuru);
void anal_input(char *shuru,char anal[][22]);
void deal_input(char a[][22]);

int main(void)
{
    signal(SIGINT,SIG_IGN);
    system("reset");
    char shuru[220];
    char anal[10][22];
    int i;
    while(1) {
        print_tishi();
        get_input(shuru);
        if(strcmp(shuru,"exit")==0) {
            goodbye();
            break;
        }
        anal_input(shuru,anal);
        deal_input(anal);
    }
    return 0;
}

void print_tishi(void)
{
    printf("myshell=> ");
}

void get_input(char *shuru)
{
    int i;
    memset(shuru,0,220);
    for(i=0;i<220;i++) {
        shuru[i] = getchar();
        if(shuru[i] == 10) {
            break;
        }
    }
    if(i == 220) {
        printf("The whole command is too long!\n");
        exit(0);
    }
    shuru[i] = '\0';
}

void anal_input(char *shuru,char anal[][22])
{
    memset(anal,0,220);
    int i=0,j=0,k=-1;
    for(i=0;i<10;i++) {
        for(j=0;j<22;j++) {
            k++;
            if(shuru[k] == ' '&&shuru[k+1] != ' ') {
                anal[i][j] = '\0';
                break;
            }
            if(shuru[k] == ' '&&shuru[k+1] == ' ') {
                j--;
                continue;
            }
            if(shuru[k] != ' ') {
                anal[i][j] = shuru[k];
            }
            if(shuru[k] == '\0') {
                break;
            }
        }
        if(j == 22) {
            printf("The alone command is too long!\n");
            exit(0);
        }
        if(shuru[k] == '\0') {
            break;
        }
    }
    if(i == 10) {
        printf("The number of command is too many!\n");
        exit(0);
    }
}

void deal_input(char a[][22])
{
    pid_t pid;
    int i;
    for(i=0;i<10;i++) {
        if(a[i][0] == '\0') {
            break;
        }
    }
//    printf("number:%d",i);
    char* arg[i+1];
    int j;
    for(j=0;j<i;j++) {
        arg[j]=(char*)a[j];
    }
    arg[i] = NULL;
    pid = fork();
    if(pid<0) {
        printf("errpr\n");
        exit(-1);
    } else if(pid>0) {
        wait(NULL);
    } else {
        signal(SIGINT,SIG_DFL);
        execvp(arg[0],arg);
    }
}

void goodbye(void)
{
    puts("************************");
    puts("*thanks for use myshell*");
    puts("*       good bye       *");
    puts("************************");

    sleep(1);
    system("clear");
}
