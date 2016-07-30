/*************************************************************************
	> File Name: 1.2.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月27日 星期三 15时55分19秒
    >>var 1.2
    >>更改提示符 username@hostname #|$
    >>历史记录
    >>自动补全
    >>上下方向键切换上/下次输入
    >>cd ~可进入该用户家目录
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
#include<readline/readline.h>
#include<readline/history.h>

void goodbye(void);
char *print_tishi(void);
void get_input(char *shuru);
void anal_input(char *shuru,char anal[][22]);
void deal_input(char a[][22]);
void cd(char *path);
void start(void);
void save_history(char *shuru);
void view_history(void);
void del_history(void);
void find_path(void);
void help(void);

char U_name[10];
char home_path[30];

int main(void)
{
    system("printf \"\ec\"");
    signal(SIGINT,SIG_IGN);
    start();
    sleep(1);
    system("printf \"\ec\"");
    char *shuru;
    char anal[10][22];
    int i;
    while(1) {

        shuru = print_tishi();
        save_history(shuru);
        if(strcmp(shuru,"exit")==0) {
            free(shuru);
            goodbye();
            break;
        } else if(strcmp(shuru,"history") == 0) {
            view_history();
            continue;
        } else if(strcmp(shuru,"del history") == 0) {
            del_history();
            continue;
        } else if(strcmp(shuru,"help") == 0) {
            help();
            continue;
        }
        anal_input(shuru,anal);
//        for(i=0;i<10;i++) puts(anal[i]);
        if(strcmp(anal[0],"cd") == 0) {
            if(strcmp(anal[1],"~") == 0) {
                find_path();
                strcpy(anal[1],home_path);
            }
            cd(anal[1]);
            free(shuru);
            continue;
        }
        deal_input(anal);
        free(shuru);
    }
    return 0;
}

char *print_tishi(void)
{
    char *line;
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
    fd1= open("/tmp/host_name",O_RDONLY);
    read(fd1,name,10);
    close(fd2);
    remove("/tmp/hostname");
    for(i=0;name[i]!='\n';i++) ;
    name[i] = '\0';
    strcpy(U_name,name);
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
//    printf("%s",re);
    line = readline(re);
    if(line!=NULL) {
        add_history(line);
    }
    return line;
}

void find_path(void)
{
    memset(home_path,30,0);
    if(strcmp(U_name,"root") == 0) {
        strcpy(home_path,"/root/");
    } else {
        strcpy(home_path,"/home/");
        strcat(home_path,U_name);
        strcat(home_path,"/");
    }
}
void save_history(char *shuru)
{
    int fd;
    find_path();
    strcat(home_path,".ksh.dat");
    if((fd = open(home_path,O_WRONLY|O_APPEND)) == -1) {
        fd = creat(home_path,0644);
        close(fd);
        fd =open(home_path,O_WRONLY|O_APPEND);
    }
    write(fd,shuru,strlen(shuru));
    write(fd,"\n",1);
    close(fd);
}

void view_history(void)
{
    find_path();
    strcat(home_path,".ksh.dat");
    int fd;
    fd = open(home_path,O_RDONLY);
    char a[1];
    while(1) {
        if(read(fd,a,1) != 0) {
            printf("%c",a[0]);
        } else {
            break;
        }
    }
//    puts("");
    close(fd);
}
void del_history(void)
{
    int fd; 
    fd = creat(home_path,0644);
    close(fd);
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

//    readline(shuru);
    if(i == 220) {
        printf("The whole command is too long!\n");
        exit(0);
    }
    shuru[i] = '\0';
//    gets(shuru);
//    if(strlen(shuru) >= 200) {
//        puts("The whole command is too long!");
//    }
}

void anal_input(char *shuru,char anal[][22])
{
    memset(anal,0,220);
    int i=0,j=0,k=-1;
    for(i=0;i<10;i++) {
        for(j=0;j<22;j++) {
            k++;
            if(shuru[k] == '\\'&&shuru[k+1] == ' ') {
				k++;
				anal[i][j] = shuru[k];
                continue;
            }
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
    if(strcmp(arg[0],"ls") == 0) {
        arg[i] = "--color=auto";
        i++;
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

void help(void)
{
    puts("welcome to us k-sh!\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\nfunction:\n    1.history:view input history\n    2.del history:delete input history\n    3.help:open help file\n    4.cd <path>:change work path\n        *cd ~:go to your home\n    5.tab:completion input\n    6.up & down:change last or next input\n    7.you can use > < | or &\n        >:Output redirection\n        <:Input redirection\n        |:Pipe symbol\n        &:Background\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\nif you have any question,you can contact me by e-mail!\ne-mail:1041984720@qq.com");

}

void goodbye(void)
{
    system("printf \"\ec\"");
    puts("◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◆◆◇◆◆◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◆◇◇◆◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◆◇◇◆◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇\n◇◇◆◆◆◆◇◇◆◆◇◇◇◇◆◆◇◇◇◇◆◆◆◇◇◆◆◆◇◇◆◆◆◇◆◆◇◇◆◆◇◇\n◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇\n◇◇◆◆◇◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◆◇◇◇◆◆◆◆◇\n◇◆◇◇◇◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◆◇◇◇◆◆◇◇◇◆◇◇◇◇\n◇◆◆◆◆◇◇◇◆◆◇◇◇◇◆◆◇◇◇◇◆◆◆◆◇◆◆◆◇◇◇◇◆◇◇◇◇◇◆◆◆◇\n◇◆◇◇◇◆◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◆◇◇◇◇◇◇◇◇◇\n◇◇◆◆◆◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◆◆◇◇◇◇◇◇◇◇◇◇\n◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇");

    sleep(1);
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

