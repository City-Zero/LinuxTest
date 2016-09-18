/*************************************************************************
	> File Name: my_ls.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月20日 星期三 11时36分32秒
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

char dangqian[100];
void rwx(int a)//权限显示函数
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

int display_l(char *path,char *change_dir)//详细显示文件
{
	struct stat buf;//文件属性结构体
	chdir(change_dir);
    //获取文件属性
    if(stat(path,&buf) == -1) {
        perror("stat:");
        exit(1);
    }

    //打印文件属性
    if(S_ISREG(buf.st_mode)) {
        printf("-");
    } else if(S_ISDIR(buf.st_mode)) {
        printf("d");
    }
	//打印文件权限
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
    printf(" %-4s %-4s",psd->pw_name,grp->gr_name);//所属用户及用户组
	printf(" %8ld",buf.st_size);
    char shijian[30];
 //   shijian = (char*)malloc(sizeof(ctime(&buf.st_mtime)));
    strcpy(shijian,ctime(&buf.st_mtime));
    int n;
    for(n=0;shijian[n] != '\n';n++) ;
    shijian[n]='\0';
    printf(" %s",shijian);
    if(S_ISDIR(buf.st_mode)) { 
        printf("\033[0;34;1m %s\033[0m:",path);
    } else printf(" %s",path);
    puts("");
	chdir(dangqian);
    return 0;
}

int my_readdir(char *path,int mod,int lin)//mod -a,lin -l
{
    DIR *dir;
    struct dirent *ptr;
    int name_len=0;
    int num=0;
    int i,j;
    char **f;
    char *temp;
    if((dir = opendir(path)) == NULL) {
//	        perror ("opendir");
		puts("dasdadsasd");
        return -1;
    }
    while ((ptr = readdir(dir)) != NULL) {
        num++;
        if(strlen(ptr->d_name) > name_len) {
            name_len = strlen(ptr->d_name);
        }
    }
    closedir(dir);

    temp = (char*)malloc(sizeof(char)*name_len);
    dir = opendir(path);
    f = (char **)malloc(sizeof(char*)*num);
    for(i=0;i<num;i++) {
        f[i] = (char *)malloc(sizeof(char)*(name_len+1));
    }
    i=0;
    while ((ptr = readdir(dir)) != NULL) {
        strcpy(f[i],ptr->d_name);
        i++;
    }
    for(i=0;i<num-1;i++) {
        for(j=0;j<num-i-1;j++) {
            if(strcmp(f[j],f[j+1]) > 0) {
                strcpy(temp,f[j]);
                strcpy(f[j],f[j+1]);
                strcpy(f[j+1],temp);
            }
        }
    }
    char na[3];
    if(name_len > 9) {
        na[0] = (name_len/10) + 48;
        na[1] = (name_len%10) + 48;
        na[2] = '\0';
    }
    else {
        na[0] = name_len + 48;
        na[1] = '\0';
    }
    char a[10]="%-";
    strcat(a,na);
    char s[]="s ";
    strcat(a,s);
	if(lin) {
		if(mod) {
			for(i=0;i<num;i++) {
				display_l(f[i],path);
			}		
		} else {
			for(i=0;i<num;i++) {
				if(f[i][0] == '.') {
					continue;
				} else {
					display_l(f[i],path);
				}
			}
			}
	} else {
		if(mod) {
    		for(i=0;i<num;i++) {
        		printf(a,f[i]);
    		}
		} else {
			for(i=0;i<num;i++) {
				if(f[i][0] == '.') {
					continue;
				} else {
					printf(a,f[i]);
				}
			}
		}
	}

    return 0;
}

int display_a(char *path)
{
	
}
int main(int argc,char **argv)
{
	int i,j;
	getcwd(dangqian,100);
    if(argc == 1) {//无参数,显示当前目录下的文件(不含隐藏)
		my_readdir("./",0,0);
		puts("");
    }
    else if(argc > 1) {
        if(strcmp(argv[1],"-l") == 0) {//详细显示文件
            if(argc == 2) {
				my_readdir("./",0,1);
            } else {
				for(i=2;i<argc;i++) {
					struct stat buf;//文件属性结构体

  			  		//获取文件属性
  					if(stat(argv[i],&buf) == -1) {
	   			   		printf("no such file or dir:%s\n",argv[i]);
  				    	exit(1);
 					}
	
  				  	//打印文件属性
  				  	if(S_ISREG(buf.st_mode)) {
  				      	display_l(argv[i],"./"); 
					} else if(S_ISDIR(buf.st_mode)) {
  				      	printf("\033[0;34;1m%s\n\033[0m:",argv[i]);
						my_readdir(argv[i],0,1);
 				   	}
					puts("\n");
				}
			}
        }
        else if(strcmp(argv[1],"-a") == 0) {//显示所有文件
			if(argc == 2) {
				my_readdir("./",1,0);
			} else {
				for(i=2;i<argc;i++) {
					struct stat buf;//文件属性结构体

  			  		//获取文件属性
  					if(stat(argv[i],&buf) == -1) {
	   			   		printf("no such file or dir:%s\n",argv[i]);
  				    	exit(1);
 					}
	
  				  	//打印文件属性
  				  	if(S_ISREG(buf.st_mode)) {
  				      	display_l(argv[i],"./"); 
					} else if(S_ISDIR(buf.st_mode)) {
  				      	printf("\033[0;34;1m%s\n\033[0m:",argv[i]);
						my_readdir(argv[i],1,0);
 				   	}
					puts("\n");
				}
			}
        }
        else if(strcmp(argv[1],"-al") ==0||strcmp(argv[1],"-la") == 0) {//详细显示隐藏文件
			if(argc == 2) {
				my_readdir("./",1,1);
			} else {
				for(i=2;i<argc;i++) {
					struct stat buf;//文件属性结构体

  			  		//获取文件属性
  					if(stat(argv[i],&buf) == -1) {
	   			   		printf("no such file or dir:%s\n",argv[i]);
  				    	exit(1);
 					}
	
  				  	//打印文件属性
  				  	if(S_ISREG(buf.st_mode)) {
  				      	display_l(argv[i],"./"); 
					} else if(S_ISDIR(buf.st_mode)) {
  				      	printf("\033[0;34;1m%s\n\033[0m:",argv[i]);
						my_readdir(argv[i],1,1);
 				   	}
					puts("\n");
				}
			}
        }
        else if(strcmp(argv[1],"-R") == 0) {//列出所有子目录层
            
        }
        else {//普通模式显示指定文件夹下的文件
            for(i=1;i<argc;i++) {
				struct stat buf;//文件属性结构体

  			  	//获取文件属性
  				if(stat(argv[i],&buf) == -1) {
   			   		printf("no such file or dir:%s\n",argv[i]);
  			    	exit(1);
 				}

  			  	//打印文件属性
  			  	if(S_ISREG(buf.st_mode)) {
  			      	printf("%s",argv[i]); 
				} else if(S_ISDIR(buf.st_mode)) {
  			      	printf("\033[0;34;1m%s\n\033[0m:",argv[i]);
					my_readdir(argv[i],0,0);
 			   	}
				puts("\n");
			}
        }
    }
}
