/*************************************************************************
	> File Name: my_client.c
	> Author: 
	> Mail: 
	> Created Time: 2016年08月02日 星期二 08时54分18秒
 ************************************************************************/

#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"my_recv.h"

#define INVALID_USERINFO  'n'  //用户信息无效
#define VALID_USERINFO     'y' //用户信息有效

//获取用户输入存入到buf,buf 的长度为len , 用户输入数据以 '\n'为结束符
int get_userinfo(char *buf ,int len)
{
    int i;
    int c;

    if(buf ==NULL)
    {
        return -1;
    }

    i = 0;
    while(((c=getchar())!='\n')&&(c!=EOF)&&(i<len-2))
    {
        buf[i++] = c;
    }
    buf[i++] = '\n';
    buf[i++] = '\0';
    return 0;
}

//输入用户名，然后通过fd发送过去

void input_userinfo(int conn_fd,const char *string)
{
    char input_buf[32];
    char recv_buf[BUFSIZE];
    int   flag_userinfo;

    //输入用户信息，直到正确为止
    do
    {
        printf("%s:",string);
        if(get_userinfo(input_buf,32)<0)
        {
            printf("error return from get_userinfo \n");
            exit(1);
        }

        if(send(conn_fd,input_buf,strlen(input_buf),0)<0)
        {
            my_err("send",__LINE__);
        }

        //从连接套接字上读取一次数据
        if(my_recv(conn_fd,recv_buf,sizeof(recv_buf))<0)
        {
            printf("data is too long\n");
            exit(1);
        }

        if(recv_buf[0] ==VALID_USERINFO)
        {
            flag_userinfo = VALID_USERINFO;
        }
        else
        {
            printf("%s error ,input again\n",string);
            flag_userinfo = INVALID_USERINFO;
        }
    }while(flag_userinfo ==INVALID_USERINFO);
}


int main(int argc,char **argv)
{
    int i;
    int ret;
    int conn_fd;
    int serv_port;
    struct sockaddr_in  serv_addr;
    char  recv_buf[BUFSIZE];

    //检查个数
    if(argc != 5)
    {
        printf("Usage:[-p][serv_port][-a][serv_addr]\n");
        exit(1);
    }


    //初始化服务器端地址结构
    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    //从命令行获取服务端的端口和地址
    for(i=1;i<argc;i++)
    {
        if(strcmp("-p",argv[i])==0)
        {
            serv_port = atoi(argv[i+1]);
            if(serv_port < 0 || serv_port >65535)
            {
                printf("invalid serv_addr.sin_port");
                exit(1);
            }
            else
            {
                serv_addr.sin_port = htons(serv_port);
            }
            continue;
        }


        if(strcmp("-a",argv[i])==0)
        {
            if(inet_aton(argv[i+1],&serv_addr.sin_addr)== 0)
            {
                printf("invalid server ip address \n");
            }
            continue;
        }
    }

    //检测是否少输入了某项参数
    if(serv_addr.sin_port == 0 || serv_addr.sin_addr.s_addr ==0)
    {
        printf("Usage:[-p][serv_addr.sin_port][-a][serv_address]\n");
        exit(1);
    }

    //创建一个TCP套接字
    conn_fd = socket(AF_INET,SOCK_STREAM,0);
    if(conn_fd < 0)
    {
        my_err("socket",__LINE__);
    }

    //向服务器端发送连接请求

    if(connect(conn_fd,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr))<0)
    {
        my_err("connnect",__LINE__);
    }

    //输入用户名和密码
    input_userinfo(conn_fd,"username");
    input_userinfo(conn_fd,"password");

    //读取欢迎信息并打印出来
    if((ret = my_recv(conn_fd,recv_buf,sizeof(recv_buf)))<0)
    {
        printf("data is to long\n");
        exit(1);
    }

    for(i=0;i<ret;i++)
    {
        printf("%c",recv_buf[i]);
    }
    printf("\n");

    close(conn_fd);
    return 0;

}
