/*************************************************************************
	> File Name: 
	> Author: 
	> Mail: 
	> Created Time: 2016年07月19日 星期二 11时27分27秒
 ************************************************************************/

#include<stdio.h>
#include<time.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/types.h>
#include<pwd.h>
#include<grp.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>

void rwx(int a)
{
    if(a == 0) {
        printf("---");
    } else if(a == 1) {
        printf("--x");
    } else if(a == 2) {
        printf("-w-");
    } else if(a == 4) {
        printf("r--");
    } else if(a == 3) {
        printf("-wx");
    } else if(a == 5) {
        printf("r-x");
    } else if(a == 6) {
        printf("rw-");
    } else if(a == 7) {
        printf("rwx");
    }
}

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
    if(S_ISREG(buf.st_mode)) {
        printf("-");
    } else if(S_ISDIR(buf.st_mode)) {
        printf("d");
    }
    int i;
    i=(buf.st_mode % 512)/64;
    rwx(i);
    i=(buf.st_mode%64)/8;
    rwx(i);
    i=buf.st_mode%8;
    rwx(i);
    printf("%4ld",buf.st_nlink);

    //打印所属用户及用户组
    struct passwd *psd;//这个结构体获取所有者用户名
    struct group *grp;//获取所属组用户名
    psd = getpwuid(buf.st_uid);
    grp = getgrgid(buf.st_gid);
    printf(" %-4s %-4s",psd->pw_name,grp->gr_name);
    char *shijian;
    shijian = (char*)malloc(sizeof(ctime(&buf.st_mtime)));
    strcpy(shijian,ctime(&buf.st_mtime));
    int n;
    for(n=0;shijian[n] != '\n';n++) ;
    shijian[n]='\0';
    printf(" %s",shijian);
    if(S_ISDIR(buf.st_mode)) { 
        printf("\033[0;34;1m %s\033[0m",argv[1]);
    } else printf(" %s",argv[1]);
    puts("");

    return 0;
}
