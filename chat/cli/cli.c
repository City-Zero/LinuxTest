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

#define Sign_In             1
#define Sign_Up             2
#define Chat_One            3
#define Chat_Group          4
#define List_All            5
#define List_my             6
#define List_group          7
#define Add_friend          8
#define Add_groum           9

typedef struct message
{
    int mode;//模式
    int resault;//状态
    char to[20];
    char from[20];
    char detail[255];
    struct message *next;
}MES;

typedef struct User_list
{
    char name[20];
    int num;
    struct User_list *next;
}U_L;

void my_err(const char *err_string,int line);
void List_Add(MES mes);
int List_View(void);
int list_all(int n);
void Search_List(int a);
void Sign_Ok(void);
int Sel(void);
void SignUp(void);
void host(void);
void chat(void);
void read_all(MES mes);
void get_one_chat(MES mes);
void add_friend(void);
void deal_friend(MES mes);
void add_mhead(MES mes);

char to_who[20];
char i_am[20];
int ser_fd;
U_L *head;//用户列表头指针
MES *mhead;//消息盒子头指针

void my_err(const char *err_string,int line)
{
    fprintf(stderr,"line:%d",line);
    perror(err_string);
    exit(1);
}

void read_all(MES mes)
{
    //puts(mes.detail);
	List_Add(mes);//结果为0为接收完毕
}

void destory(void) //销毁链表
{
    U_L *temp,*temp2;
    temp=head;
    temp2=temp->next;
    while(temp!=NULL) {
        temp2=temp->next;
        free(temp);
        temp=temp2;
    }
}

void get_one_chat(MES mes)
{
    //接收用户消息
    //printf("mes.from->%s mes.to->%s i_am->%s to_who->%s\n",mes.from,mes.to,i_am,to_who);
    if(strcmp(mes.from,to_who) == 0){
        //如果是当前用户则直接输出
        printf("\t\t%s",mes.detail);
        printf(YELLOW" <-%s"WHITE,mes.from);
        puts("");
    }else{
        //不是当前聊天的用户就放进消息盒子
    }
}

void my_recv(void)
{
	//只用来接收信息的子线程
	while(1) {
		MES mes;
		memset(&mes,0,sizeof(MES));
		if(recv(ser_fd,&mes,sizeof(MES),0) < 0) {
			my_err("recv",__LINE__);
			exit(0);
		}
		switch(mes.mode) {
		case 3:
			//私聊消息
            get_one_chat(mes);
			break;
		case 4:
			//群聊消息
			break;
		case 5:
			//附近的人
			read_all(mes);
			break;
		case 6:
			//我的好友
			read_all(mes);
			break;
		case 7:
			//我的群
			break;
		case 8:
			//加好友
            if(mes.resault==-1) {
                printf(BLUE"\t%s请求加你为好友,请到消息盒子处理\n"WHITE,mes.from);
                add_mhead(mes);
            } else if(mes.resault == 0) {
                printf(RED"%s拒绝了你的好友请求\n"WHITE,mes.to);
            } else if(mes.resault == 1) {
                printf(RED"%s同意了你的好友请求\n"WHITE,mes.to);
            }
			break;
		case 9:
			//加群
			break;
		}
	}
}
void chat(void)
{
    MES mes;
    puts(YELLOW"-----------------------------------------------");
    puts("-                                             -");
    puts("-   输入要发送的消息,按回车键发送             -");
    puts("-                                             -");
    puts("-   输入-quit-返回上一层                      -");
    puts("-                                             -");
    puts("-----------------------------------------------"WHITE);
    while(1) {
        memset(&mes,0,sizeof(MES));
        fgets(mes.detail,sizeof(mes.detail),stdin);
        mes.detail[strlen(mes.detail)-1] = '\0';
        if(strcmp(mes.detail,"-quit-") == 0) {
            break;
        }
        setbuf(stdin,NULL);
        mes.mode = Chat_One;
        strcpy(mes.from,i_am);
        strcpy(mes.to,to_who);
        if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
            my_err("send",__LINE__);
            exit(0);
        }
    }
}

void List_Add(MES mes)
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

int List_View(void)
{
    U_L *temp;
    temp = head->next;
    while(temp != NULL) {
        printf("%d.%s\n",temp->num,temp->name);
        temp =temp->next;
    }
    return head->next->num;
}

int mess_box(void)
{
    MES *temp,*temp2;
    temp=mhead->next;
    int i=1;
    while(temp!=NULL) {
        printf("%d.",i);
        switch(temp->mode) {
        case 3:
            printf("[私聊]%s\n",temp->from);
            break;
        case 4:
            printf("[群聊]%s\n",temp->to);
            break;
        case 8:
            if(temp->resault==-1) {
                printf("%s想加你为好友\n",temp->from);
            }
            else if(temp->resault == 0) {
                printf("%s拒绝了你的交友请求",temp->to);
            }
            else if(temp->resault==1) {
                printf("%s同意了你的交友请求",temp->to);
            }
            break;
        case 9:
            printf("%s群邀请你加入\n",temp->to);
            break;
        case 10:
            break;
        case 11:
            break;
        case 12:
            break;
        }
        temp=temp->next;
        i++;
    }
    int sel;
    puts("输入数字处理消息.0返回上一层");
    do{
        sel=Sel();
    }while(sel<0||sel>=i);
    if(sel == 0) return 0;
    temp=mhead->next;
    i=1;
    while(temp!=NULL) {
        if(sel == i)
        break;
        else {
            i++;
            temp=temp->next;
        }
    }
    switch(temp->mode) {
    case 3:
        printf("[私聊]%s\n",temp->from);
        break;
    case 4:
        printf("[群聊]%s\n",temp->to);
        break;
    case 8:
        if(temp->resault==-1) {
            printf("%s请求加你为好友,是否同意(y/n)?\n",temp->from);
            char ch;
            do{
                printf("请输入:");
                ch = getchar();
            }while(ch!='y'&&ch!='n');
            MES mes;
            memcpy(&mes,temp,sizeof(MES));
            if(ch=='y') {
                puts("成功添加好友!");
                mes.resault=1;
            } else {
            puts("你以拒绝对方的请求!");
            mes.resault=0;
            }
            if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
                my_err("send",__LINE__);
                exit(0);
            }
        }else if(temp->resault == 0) {
            printf("%s拒绝了你的交友请求\n",temp->to);
        }else if(temp->resault == 1) {
            printf("%s同意了你的交友请求\n",temp->to);
        }
        break;
    case 9:
        printf("%s群邀请你加入\n",temp->to);
        break;
    case 10:
        break;
    case 11:
        break;
    case 12:
        break;
    }
}

void Sign_Ok(void)
{
    system("printf \"\ec\"");
    puts(RED"\t\t\t已登陆"WHITE);
    puts("");
    puts(BLUE"1.附近的人");
    puts("2.我的好友");
    puts("3.我的群");
    puts("4.消息盒子");
    puts("5.");
    puts("6.退出登陆"WHITE);
    puts("");
    int sel;
    do{
        sel=Sel();
    }while(sel<1||sel>6);
    switch(sel) {
    case 1:
        list_all(5);
        Sign_Ok();
        break;
    case 2:
        list_all(6);
        Sign_Ok();
        break;
    case 3:
        break;
    case 4:
        mess_box();
        Sign_Ok();
        break;
    case 5:
        break;
    case 6:
        break;
    }
}

void Search_List(int a)
{
    U_L *temp = head->next;
    while(temp != NULL) {
        if(a == temp->num) {
            strcpy(to_who,temp->name);
            break;
        }
        temp=temp->next;
    }
}
void add_friend(void)
{
    MES mes;
    memset(&mes,0,sizeof(MES));
    mes.mode = 8;
    mes.resault = -1;//首次发送请求的标志
    strcpy(mes.from,i_am);
    strcpy(mes.to,to_who);
    if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
        my_err("send",__LINE__);
        exit(0);
    }
    puts("好友请求已发送,请耐心等待对方同意...");
}

int list_all(int n)
{
    head = (U_L*)malloc(sizeof(U_L));
    head->next=NULL;
    MES mes;
    memset(&mes,0,sizeof(MES));
    mes.mode = n;
    if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
        //给服务器发送需要获取用户列表的的信息
        my_err("send",__LINE__);
        exit(0);
    }
    sleep(1);
    int max = List_View();
    int sel;
    if(n == 5) {
        puts(RED"请输入序号来添加好友,输入0返回上一层"WHITE);    
    } else if(n == 6) {
        puts(RED"请输入序号来选择好友聊天,输入0返回上一层"WHITE);
    }
    do{
        sel = Sel();
    } while(sel < 0 || sel > max);
    if(sel == 0) return 0;
    Search_List(sel);//检查所选编号所对应的用户名
    destory();
    if(strcmp(to_who,i_am) == 0) {
        puts(RED"不能选择自己"WHITE);
        return 0;
    }
    if(n == 5) {
        add_friend();    
    } else if(n == 6) {
        chat();
    }
}
int Sel(void)
{
    printf("请选择:");
    int a;
    scanf("%d",&a);
    setbuf(stdin,NULL);
    return a;
}

void SignIn(void)
{
    system("printf \"\ec\"");
    puts(PURPLE"\t\t登陆\t\t"WHITE);
    MES mes;
    char name[20];
    printf("请输入用户名:");
    scanf("%s",name);
    //fgets(name,20,stdin);
    //name[strlen(name)]='\0';
    setbuf(stdin,NULL);
    char pass[20];
    memcpy(pass,getpass("请输入密码:"),20);
    memset(&mes,0,sizeof(MES));
    mes.mode=Sign_In;
    strcpy(mes.detail,name);
    mes.detail[strlen(mes.detail)] = ';';
    mes.detail[strlen(mes.detail)+1] = '\0';
    strcat(mes.detail,pass);
    if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
        my_err("send",__LINE__);
        exit(0);
    }
    memset(&mes,0,sizeof(MES));
    if(recv(ser_fd,&mes,sizeof(MES),0) < 0) {
        my_err("recv",__LINE__);
        exit(0);
    }
    if(mes.resault) {
        puts("登陆成功,正在跳转...");
		pthread_t thid;
        pthread_create(&thid,NULL,(void*)my_recv,NULL);
        sleep(1);
        strcpy(i_am,name);
        Sign_Ok();
    } else{
        puts("登陆失败");
        sleep(1);
    }
}
void SignUp(void)
{
    system("printf \"\ec\"");
    puts(PURPLE"\t\t注册\t\t"WHITE);
    MES mes;
    char name[20];
    printf("请输入用户名:");
    scanf("%s",name);
    //fgets(name,20,stdin);
    //name[strlen(name)]='\0';
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
            mes.detail[strlen(mes.detail)] = ';';
            mes.detail[strlen(mes.detail)+1] = '\0';
            strcat(mes.detail,pass);
            break;
        } else {
            puts("两次密码不匹配!");
        }
    }
    mes.mode = Sign_Up;
    if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
        my_err("send",__LINE__);
        exit(0);
    }
    memset(&mes,0,sizeof(mes));
    if(recv(ser_fd,&mes,sizeof(MES),0) < 0) {
        my_err("recv",__LINE__);
    }
    if(mes.resault) {
        //注册成功
        puts("注册成功,正在跳转...");
		pthread_t thid;
        pthread_create(&thid,NULL,(void*)my_recv,NULL);
        sleep(1);
        strcpy(i_am,name);
        Sign_Ok();
        sleep(1);
    } else {
        puts("注册失败,用户名已存在!");
        sleep(2);
    }
}

void host(void)
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
        SignIn();
        host();
        break;
    case 2:
        SignUp();
        host();
        break; 
    }

}

void add_mhead(MES mes)
{
    MES *temp;
    temp=(MES*)malloc(sizeof(MES));
    //temp=mes;
    memcpy(temp,&mes,sizeof(MES));
    temp->next=mhead->next;
    mhead->next=temp;
}

int main(void)
{
    int conn_fd;
    struct sockaddr_in serv_addr;
    char recv_buf[255];
    int ret;
    int serv_port;
    
    mhead=(MES*)malloc(sizeof(MES));
    mhead->next=NULL;
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
	ser_fd = conn_fd;
    host();
}
