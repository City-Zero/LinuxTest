/*************************************************************************
	> File Name: 1.2.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月27日 星期三 15时55分19秒
    >>var 1.2
    >>加入&和|
    >>加入启动结束画面
    >>加入错误提示
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<dirent.h>
#include<signal.h>
#include<errno.h>

void goodbye(void);
void print_tishi(void);
void get_input(char *shuru);
void anal_input(char *shuru,char anal[][22]);
void deal_input(char a[][22]);
void cd(char *path);
void start(void);

int main(void)
{
    system("printf \"\ec\"");
    signal(SIGINT,SIG_IGN);
    start();
    sleep(2);
    system("printf \"\ec\"");
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
        if(strcmp(anal[0],"cd") == 0) {
            cd(anal[1]);
            continue;
        }
        deal_input(anal);
    }
    return 0;
}

void print_tishi(void)
{
    printf("K-sh@ ");
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
    int bk=0;
    int mode=0;
    int mode_num=0;
    pid_t pid,pid2;
    int i;
    char *file_name;
    int fd,fd2;
    for(i=0;i<10;i++) {
        if(a[i][0] == '\0') {
            break;
        }
        if(strlen(a[i])>1) {
            continue;
        }
        if(strcmp(a[i],">")==0) {
            mode_num++;
            mode=1;
        } else if(strcmp(a[i],"<")==0) {
            mode_num++;
            mode=2;
        } else if(strcmp(a[i],"|")==0) {
            mode_num++;
            mode=3;
        }
    }
    if(strcmp(a[i-1],"&") == 0) {
        i--;
        bk = 1;
    }
    if(mode_num>1) {
        puts("Too much symbol!");
        return;
    }
    char* arg[i+1];
    int j;
    for(j=0;j<i;j++) {
        arg[j]=(char*)a[j];
    }
    arg[i] = NULL;
    if("mode == 1||mode ==2") {
        for(j=0;j<i;j++) {
            if((strcmp(arg[j],">") == 0)||(strcmp(arg[j],"<") == 0)) {
                arg[j] = NULL;
                file_name = (char*)malloc(strlen(arg[j+1])+1);
                strcpy(file_name,arg[j+1]);
                break;
            }
        }
    } 
    if(mode == 3) {
        for(j=0;j<i;j++) {
            if(strcmp(arg[j],"|") == 0) {
                arg[j] = NULL;
                j++;
                break;
            }
        }
    }
    pid = fork();
    if(pid<0) {
        printf("errpr\n");
        exit(-1);
    } else if(pid>0) {
        if(bk) {
            printf("[process is %d]\n",pid);
            return;
        } else{
            wait(NULL);
        }
    } else {
        signal(SIGINT,SIG_DFL);
        if(mode == 0) {
            if(execvp(arg[0],arg) == -1) {
                perror(arg[0]);
            }
        } else if(mode == 1) {
            fd = open(file_name,O_RDWR|O_CREAT|O_TRUNC,0644);
            dup2(fd,1);
            if(execvp(arg[0],arg) == -1) {
                perror(arg[0]);
            }
        } else if(mode == 2) {
            fd = open(file_name,O_RDONLY);
            dup2(fd,0);
            if(execvp(arg[0],arg) == -1) {
                perror(arg[0]);
            }
        } else if(mode == 3) {
            pid2=fork();
            if(pid2==0) {
                remove("/tmp/my_shell");
                fd = open("/tmp/my_shell",O_RDWR|O_CREAT|O_TRUNC,0644);
                dup2(fd,1);
                if(execvp(arg[0],arg) == -1) {
                    perror(arg[0]);
                }
                exit(0);
            }else if(pid2>0) {
                wait(NULL);
                fd = open("/tmp/my_shell",O_RDONLY);
                dup2(fd,0);
                int k=0;
                for(i=j;arg[i]!=NULL;i++) {
                    k++;
                }
                char *arg2[k+1];
                for(j,i=0;arg[j]!=NULL;j++) {
                    arg2[i]=arg[j];
                    i++;
                }
                arg2[k]=NULL;
                if(execvp(arg2[0],arg2) == -1) {
                    perror(arg2[0]);
                }
                exit(0);
            }
        }
        exit(0);
    }
}

void goodbye(void)
{
    system("printf \"\ec\"");
    puts("◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◆◆◇◆◆◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◆◇◇◆◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◆◇◇◆◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◆◆◆◆◇◇◆◆◇◇◇◇◆◆◇◇◇◇◆◆◆◇◇◆◆◆◇◇◆◆◆◇◆◆◇◇◆◆◇◇\n◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇\n◇◇◆◆◇◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◆◇◇◇◆◆◆◆◇\n◇◆◇◇◇◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◇◆◆◇◇◇◆◇◇◇◇\n◇◆◆◆◆◇◇◇◆◆◇◇◇◇◆◆◇◇◇◇◆◆◆◆◇◆◆◆◇◇◇◇◆◇◇◇◇◇◆◆◆◇\n◇◆◇◇◇◆◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◆◇◇◇◇◇◇◇◇◇\n◇◇◆◆◆◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◆◆◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇");

    sleep(2);
    system("clear");
}

void cd(char *path)
{
    char *s;
    if(path[0] == '/') {
        if(chdir(path) == -1) {
            perror("cd");
        }
    } else {
        s=(char *)malloc(strlen(path)+5);
        strcpy(s,"./");
        strcat(s,path);
        if(chdir(s) == -1) {
            perror("cd");
        }
        free(s);
    }
}

void start(void)
{
    puts("◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◆◆◆◇◆◆◆◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◆◆◆◇◇◇◇◇◇◇◇◇◇\n◇◆◆◆◆◆◆◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◆◆◆◇◇◇◇◇◇◇◇◇◇\n◇◆◆◆◆◆◆◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◆◆◆◇◇◇◇◇◇◇◇◇◇\n◇◆◆◆◆◆◇◇◇◇◇◇◇◇◇◇◇◇◆◆◆◇◇◆◆◆◆◆◇◇◇◇◇◇◇◇\n◇◆◆◆◆◆◇◇◇◆◆◆◆◆◆◆◆◆◆◆◆◆◇◆◆◆◆◆◆◆◇◇◇◇◇◇\n◇◆◆◆◆◆◇◇◇◇◇◇◇◇◇◇◆◆◆◇◇◇◇◆◆◆◇◆◆◆◇◇◇◇◇◇\n◇◆◆◆◆◆◆◇◇◇◇◇◇◇◇◇◇◆◆◆◆◆◇◆◆◆◇◆◆◆◇◇◇◇◇◇\n◇◆◆◆◆◆◆◇◇◇◇◇◇◇◇◇◇◇◇◇◆◆◆◆◆◆◇◆◆◆◇◇◇◇◇◇\n◇◆◆◆◇◆◆◆◇◇◇◇◇◇◇◇◆◆◆◇◆◆◆◆◆◆◇◆◆◆◇◇◇◇◇◇\n◇◆◆◆◇◆◆◆◇◇◇◇◇◇◇◇◇◆◆◆◆◆◇◆◆◆◇◆◆◆◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇");
}

