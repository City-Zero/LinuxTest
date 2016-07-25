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
    fprintf(stderr, "line:%d",line);
    perror(err_string);
    exit(0);
}

void display_R(char *path)
{
	char *s;
    printf("\033[0;34;1m%s:\n\033[0m",path);
    chdir(path);
//        puts("change dir success");
    DIR *dir;
    struct stat buf;
    struct dirent *ptr;
    char filename[MAX];
    memset(filename, 0, MAX);
    strcpy(filename, path);
//	puts(path);
	s = getcwd(NULL,0);
	puts(filename);
	puts(s);
	free(s);
	dir = opendir("./");
	if(dir == NULL) puts("fail");
	while((ptr = readdir(dir)) !=  NULL){
		if(ptr->d_name[0] == '.') {
			continue;
		} else {
			printf("%s\t", ptr->d_name);
		}
	}
//	puts("\033[0;33;1m is over \033[0m");
	closedir(dir);
	dir = opendir("./");
    while((ptr = readdir(dir)) !=  NULL){
//		puts(ptr->d_name);
        if(stat(ptr->d_name, &buf) == -1) {
            my_err("lstat",__LINE__);
		}
        if(S_ISDIR(buf.st_mode)){//判断是否目录
//            puts("is a adir");
/*            if(filename[strlen(path) - 1] != '/'){//判断目录最后一个字是不是‘/’，防止多加‘/’，目录判断出错。
                filename[strlen(path)] = '/';
                filename[strlen(path)+1] = '\0';
            }
*/
//            puts(path);
            if(strcmp(ptr->d_name, ".") == 0)
                continue;
            if(strcmp(ptr->d_name, "..") == 0)
                continue;
//            filename[strlen(filename)] = '\0';
			strcpy(filename,"./");
            strcat(filename, ptr->d_name);
            printf("\033[0;33;1m%s\n\033[0m", filename);
            display_R(filename);//递归
            
        } 
    }
	chdir("..");//函数执行完后返回，工作目录已改变。
            memset(filename, 0, MAX);//目录清空
            strcpy(filename, path);
    closedir(dir);
}


int main(int argc, char **argv)
{
    if(argc != 2) {
        printf("./ls_R <path>");
        exit(0);
    }
    display_R(argv[1]);
	puts("");
}
