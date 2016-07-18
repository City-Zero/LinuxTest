/*************************************************************************
	> File Name: 6.2.3-chmod.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月18日 星期一 14时41分53秒
    >权限修改主要是调用了chmod()函数,由于main()函数传入的参数为字符串,所以用atoi()函数进行转换,将10进制手动转为8进制后即可简单的调用chmod()函数
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>

int main(int argc,char ** argv)
{
    int mode;//权限
    int mode_u;//所有者权限
    int mode_g;//所属组权限
    int mode_o;//其他用户权限
    char *path;

    //检查参数合法性
    //三个参数 0.执行文件名,1.权限,2文件名
    if(argc < 3) {
        printf("%s <mode num> <target file>\n",argv[0]);
        exit(0);
    }

    //获取命令行参数
    mode = atoi(argv[1]);//字符串转整形
    if(mode > 777||mode < 0) {
        printf("mode num error!\n");
        exit(0);
    }
    mode_u = mode / 100;
    mode_g =( mode % 100 ) / 10;
    mode_o = mode % 10;
    mode = (mode_u * 8 * 8) + (mode_g * 8) + mode_o;//八进制转换
    path = argv[2];

    if(chmod(path,mode) == -1) {
        perror("chmod error");
        exit(1);
    }

    return 0;
}
