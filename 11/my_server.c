/*************************************************************************
	> File Name: my_server.c
	> Author: 
	> Mail: 
	> Created Time: 2016年08月02日 星期二 14时08分26秒
 ************************************************************************/

#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include"my_recv.h"

#define SERV_PORT  4040
#define LISTENQ    255

#define  INVALID_USERNAME  'n'
#define  VALID_USERNAME    'y'

#define  USERNAME   0
#define  PASSWORD   1
struct userinfo
{
    char username[32];
    char password[32];
};
struct userinfo users[]=
{
    {"linux","unix"},
    {"4507","4508"},
    {"clh","clh"},
    {" "," "}
};

int find_name(const char *name)
{
    int i;
    if(name == NULL )
    {
        printf("in find_name ,NULL pointer ");
        return -2;
    }
    for(i=0;users[i].username[0]!=' ';i++)
    {
        if(strcmp(users[i].username,name)==0)
        {
            return i;
        }
    }
    return -1;
}

void send_data(int conn_fd,const char *string)
{
    if(send(conn_fd,string,strlen(string),0)<0)
    {
        my_err("send",__LINE__);
    }
}

int main()
{
    int sock_fd,conn_fd;
    int optval;
    int flag_recv = USERNAME;
    int ret;
    int  name_num;
    pid_t pid;
    socklen_t   cli_len;
    struct sockaddr_in  cli_addr,serv_addr;//TCP/IP族协议地址结构体
    char recv_buf[128];

    sock_fd = socket(AF_INET,SOCK_STREAM,0);//使用ipv4协议创建TCP流套接字
    if(sock_fd <  0)
    {
        my_err("socket",__LINE__);
    }
    
    optval = 1;
    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(void*) &optval,sizeof(int))<0)
    {
        my_err("setsockopt",__LINE__);
    }

    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;//地址类型
    serv_addr.sin_port = htons(SERV_PORT);//端口号(宏定义)
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);//IP(本机上的任意)

    if(bind(sock_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr_in))<0)//绑定套接字(次函数用在服务器端)
    {
        my_err("bind",__LINE__);
    }

    if(listen(sock_fd,LISTENQ)<0)//转为监听套接字(监听队列长度[宏定义])
    {
        my_err("listen",__LINE__);
    }

    cli_len = sizeof(struct sockaddr_in);
    while(1)
    {
        conn_fd = accept(sock_fd,(struct sockaddr*)&cli_addr,&cli_len);//接受连接,第二个参数保存发起连接的主机地址和端口,返回一个代表新客户端的接口
        if(conn_fd < 0)
        {
            my_err("accept",__LINE__);
        }

        printf("accept a new client , ip :%s\n",inet_ntoa(cli_addr.sin_addr));//打印客户端地址

        if((pid =fork())==0)
        {
            while(1)
            {
                if((ret = recv(conn_fd,recv_buf,sizeof(recv_buf),0))<0)//接收参数1指定套接字描述符的数据并保存到参数2,返回接收到的数据量
                {
                    perror("recv");
                    exit(1);
                }
                recv_buf[ret-1] = '\0';//结束符

                if(flag_recv == USERNAME)
                {
                    name_num = find_name(recv_buf);
                    switch(name_num)
                    {
                        case -1:
                            send_data(conn_fd,"n\n");//自定义发送数据函数
                            break;
                        case -2:
                            exit(1);
                            break;
                        default:
                            send_data(conn_fd,"y\n");
                            flag_recv = PASSWORD;
                            break;
                    }
                }
                else if(flag_recv == PASSWORD)
                {
                    if(strcmp(users[name_num].password,recv_buf) == 0)
                    {
                        send_data(conn_fd,"y\n");
                        send_data(conn_fd,"Welcome login my tcp server\n");
                        printf("%s login\n",users[name_num].username);
                        break;
                    }
                    else
                        send_data(conn_fd,"n\n");
                } 
            }
            close(sock_fd);
            close(conn_fd);
            exit(0);
        }
        else
        {
            close(conn_fd);
        }
    }
    return 0;
}
