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
#define SERV_IP "127.0.0.1"
#define SERV_PORT 4040

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
        if(strcmp(string,"username") == 0) {
            printf("%s:",string);
            if(get_userinfo(input_buf,32)<0)
            {
                printf("error return from get_userinfo \n");
                exit(1);
            }
        } else {
            char *pa;
            pa=getpass("password:");
            pa[strlen(pa)] = '\n';
            pa[strlen(pa)+1] = '\0';
            strcpy(input_buf,pa);
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
int Sel(void)
{
    printf("please select:");
    int a;
    scanf("%d",&a);
//    a = _getch();
    getchar();
    return a;
}
void sign_in(int conn_fd)
{
    system("printf \"\ec\"");
    puts("sign in");
    input_userinfo(conn_fd,"username");
    input_userinfo(conn_fd,"password");

}
void host(int conn_fd)
{
    int sel;
    system("printf \"\ec\"");
    puts("welcome use chat room,you can:");
    puts("1.sign in");
    puts("2.sign up");
    do {
        sel=Sel();
    } while(sel<0||sel>2);
    switch (sel) {
    case 1:
        sign_in(conn_fd);
        break;
    case 2:
        break;
    }
}


int main(int argc,char **argv)
{
    int i;
    int ret;
    int conn_fd;
    int serv_port;
    struct sockaddr_in  serv_addr;
    char  recv_buf[BUFSIZE];
    
    //初始化服务器端地址结构
    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    //绑定IP和端口
    serv_addr.sin_port = htons(SERV_PORT);
    inet_aton(SERV_IP,&serv_addr.sin_addr);

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
    host(conn_fd);
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
