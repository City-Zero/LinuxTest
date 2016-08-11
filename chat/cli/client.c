/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: 2016年08月06日 星期六 08时14分00秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<stdlib.h>

#define HostIp      "127.0.0.1"
#define HostPort    4040

#define WHITE       "\e[0m"
#define BLACK       "\e[1;30m"
#define RED         "\e[1;31m"
#define GREEN       "\e[1;32m"
#define YELLOW      "\e[1;33m"
#define BLUE        "\e[1;34m"
#define PURPLE      "\e[1;35m"

#define Sign_In      1
#define Sign_Up      2
#define Chat_        3
#define List_        4

typedef struct message
{
    int mode;//模式
    int resault;//状态
    char to[20];
    char from[20];
    char detail[255];
}MES;

typedef struct User_list
{
    char name[20];
    int num;
    struct User_list *next;
}U_L;

void my_err(const char *err_string,int line);
void List_Add(U_L *head,MES mes);
int List_View(U_L *head);
void Search_List(int a,U_L *head);
int SigOK(int conn_fd);
int Sel(void);
void SignUp(int conn_fd);
void host(int conn_fd);
void chat(int conn_fd);

char to_who[20];
char i_am[20];

void my_err(const char *err_string,int line)
{
    fprintf(stderr,"line:%d",line);
    perror(err_string);
    exit(1);
}

void chat(int conn_fd)
{
    MES mes;
    puts(YELLOW"-----------------------------------------------");
    puts("-                                             -");
    puts("-   input your message,press enter to send    -");
    puts("-   input \"-logout-\" to downlin and quit    -");
    puts("-   input \"-host-\" return to user list      -");
    puts("-                                             -");
    puts("-----------------------------------------------"WHITE);
    while(1) {
        memset(&mes,0,sizeof(MES));
        printf(PURPLE"->%s :"WHITE,to_who);
        fgets(mes.detail,sizeof(mes.detail),stdin);
        mes.detail[strlen(mes.detail)] = '\0';
        if(strcmp(mes.detail,"-logout-") == 0) {
            exit(0);
        } else if(strcpy(mes.detail,"-host-") == 0) {
            SigOK(conn_fd);
            exit(0);
        }
        setbuf(stdin,NULL);
        mes.mode = Chat_;
        strcpy(mes.from,i_am);
        strcpy(mes.to,to_who);
        if(send(conn_fd,&mes,sizeof(MES),0) < 0) {
            my_err("send",__LINE__);
            exit(0);
        }
    }
}

void List_Add(U_L *head,MES mes)
{
    U_L *temp;
    temp = (U_L*)malloc(sizeof(U_L));
    if(head->next == NULL) {
        temp->num = 1;
    } else {
        temp->num = head->next->num + 1;
    }
    strcpy(temp->name,mes.detail);
    temp->next=head->next;
    head->next=temp;
}

int List_View(U_L *head)
{
    U_L *temp;
    temp = head->next;
    while(temp != NULL) {
        printf("%d.%s",temp->num,temp->name);
        temp =temp->next;
    }
    return head->next->num;
}
void Search_List(int a,U_L *head)
{
    U_L *temp = head->next;
    while(temp != NULL) {
        if(a == temp->num) {
            strcpy(to_who,temp->name);
            return;
        }
    }
}
int SigOK(int conn_fd)
{
    U_L *head;
    head = (U_L*)malloc(sizeof(U_L));
    head->next=NULL;
    MES mes;
    memset(&mes,0,sizeof(MES));
    mes.mode = 4;
    if(send(conn_fd,&mes,sizeof(MES),0) < 0) {//给服务器发送需要获取用户列表的的信息
        my_err("send",__LINE__);
        exit(0);
    }
    do {
        memset(&mes,0,sizeof(MES));
        if(recv(conn_fd,&mes,sizeof(MES),0) < 0) {
            my_err("recv",__LINE__);
            exit(0);
        }else { //将用户加入一个链表
            List_Add(head,mes);
        }
    } while(mes.resault);//结果为0为接收完毕
    int max = List_View(head);
    int sel;
    do{
        sel = Sel();
    } while(sel < 1 || sel > max);
    Search_List(sel,head);//检查所选编号所对应的用户名
    chat(conn_fd);
}
int Sel(void)
{
    printf("请选择:");
    int a;
    scanf("%d",&a);
    setbuf(stdin,NULL);
    return a;
}

void SignIn(int conn_fd)
{
    system("printf \"\ec\"");
    puts(PURPLE"\t\t登陆\t\t"WHITE);
    MES mes;
    char name[20];
    printf("请输入用户名:");
    fgets(name,20,stdin);
    setbuf(stdin,NULL);
    char pass[20];
    memcpy(pass,getpass("请输入密码:"),20);
    memset(&mes,0,sizeof(MES));
    mes.mode=Sign_In;
    strcpy(mes.detail,name);
    mes.detail[strlen(mes.detail)-1] = ';';
    mes.detail[strlen(mes.detail)] = '\0';
    strcat(mes.detail,pass);
    if(send(conn_fd,&mes,sizeof(MES),0) < 0) {
        my_err("send",__LINE__);
        exit(0);
    }
    memset(&mes,0,sizeof(MES));
    if(recv(conn_fd,&mes,sizeof(MES),0) < 0) {
        my_err("recv",__LINE__);
        exit(0);
    }
    if(mes.resault) {
        puts("登陆成功,正在跳转...");
        sleep(1);
        strcpy(i_am,name);
        SigOK(conn_fd);
    } else{
        puts("登陆失败");
        sleep(1);
    }
}
void SignUp(int conn_fd)
{
    system("printf \"\ec\"");
    puts(PURPLE"\t\t注册\t\t"WHITE);
    MES mes;
    char name[20];
    printf("请输入用户名:");
    fgets(name,20,stdin);
    setbuf(stdin,NULL);
    while(1) {
        char pass[20];
        char passp[20];
        memcpy(pass,getpass("请输入密码:"),20);
        setbuf(stdin,NULL);
        memcpy(passp,getpass("请再次输入:"),20);
        setbuf(stdin,NULL);
        if(strcmp(pass,passp) == 0) {
            memset(&mes,0,sizeof(mes));
            strcpy(mes.detail,name);
            mes.detail[strlen(mes.detail)-1] = ';';
            mes.detail[strlen(mes.detail)] = '\0';
            strcat(mes.detail,pass);
            break;
        } else {
            puts("两次密码不匹配!");
        }
    }
    mes.mode = Sign_Up;
    if(send(conn_fd,&mes,sizeof(MES),0) < 0) {
        my_err("send",__LINE__);
        exit(0);
    }
    memset(&mes,0,sizeof(mes));
    if(recv(conn_fd,&mes,sizeof(MES),0) < 0) {
        my_err("recv",__LINE__);
    }
    if(mes.resault) {
        //注册成功
        puts("注册成功,正在跳转...");
        sleep(1);
        strcpy(i_am,name);
        SigOK(conn_fd);
        sleep(1);
    } else {
        puts("注册失败,用户名已存在!");
        sleep(2);
    }
}

/*
void SignIn(int conn_fd)
{
    system("printf \"\ec\"");
    puts(PURPLE"\t\t登陆\t\t"WHITE);
    MES mes;
    char name[20];
    printf("请输入用户名:");
    fgets(name,20,stdin);
    name[strlen(name)] = '\0';
    char pass[20];
    memcpy(pass,getpass("请输入密码:"),20);
    memset(&mes,0,sizeof(mes));
    strcpy(mes.detail,name);
    strcat(mes.detail,";");
    strcat(mes.detail,pass);
    mes.mode = Sign_In;
    if(send(conn_fd,&mes,sizeof(MES),0) < 0) {
        my_err("send",__LINE__);
        exit(0);
    }
    memset(&mes,0,sizeof(MES));
    if(recv(conn_fd,&mes,sizeof(MES),0) < 0) {
        my_err("recv",__LINE__);
        exit(0);
    }
    if(mes.mode == Sign_In && mes.resault == 1) {
        strcpy(i_am,name);
        puts(mes.detail);
    } else {
        puts(mes.detail);
        SignIn(conn_fd);
    }

}
*/
void host(int conn_fd)
{
        int sel;
        system("printf \"\ec\"");
        puts(RED"欢迎来到了聊天室,你可以:"WHITE);
        puts(PURPLE"1"WHITE".登陆");
        puts("");
        puts(BLUE"2"WHITE".注册");
        puts("");
    do {
        sel=Sel();    
    } while(sel<0||sel>2);
    switch (sel) {
    case 1:
        SignIn(conn_fd);
        host(conn_fd);
        break;
    case 2:
        SignUp(conn_fd);
        host(conn_fd);
        break; 
    }

}

int main(void)
{
    int conn_fd;
    struct sockaddr_in serv_addr;
    char recv_buf[255];
    int ret;
    int serv_port;

    //初始化服务器信息
    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(HostPort);
    inet_aton(HostIp,&serv_addr.sin_addr);
    //创建TCP套接字
    conn_fd = socket(AF_INET,SOCK_STREAM,0);
    if(conn_fd < 0) {
        my_err("socket",__LINE__);
    }

    //发送连接请求
    if(connect(conn_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) < 0) {
        my_err("connect",__LINE__);
    }

    //主菜单
    host(conn_fd);
}
