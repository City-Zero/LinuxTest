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
#define Add_group           9
#define LogOut              10
#define One_History         11
#define Gro_History         12
#define Del_friend          13
#define Cha_pass            14
#define Cre_group           15
#define Del_user            16
#define Exit_group          17
#define Group_user          18

typedef struct message
{
    int mode;//模式
    int resault;//状态
    char to[20];
    char from[20];
    char detail[255];
    int num;
    struct message *next;
    struct message *sam_next;//同种消息指针
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
void chat(MES *temp);
void get_one_chat(MES mes);
void add_friend(void);
void deal_friend(MES mes);
void add_mhead(MES mes);
void change_pass(void);
void creat_group(void);
int add_group(char *name);
void tishi_qun(void);
void ti_ren(void);
void get_group_chat(MES mes);

char to_who[20];
char i_am[20];
int ser_fd;
U_L *head;//用户列表头指针
MES *mhead;//消息盒子头指针
int online=1;
char qun_name[20];

void my_err(const char *err_string,int line)
{
    fprintf(stderr,"line:%d",line);
    perror(err_string);
    exit(1);
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

void get_group_chat(MES mes)
{
    if(strcmp(mes.to,qun_name) == 0){
        //如果是当前群则直接输出
        printf("\t\t%s",mes.detail);
        printf(YELLOW" <-%s"WHITE,mes.from);
        puts("");
    }else{
            printf(PURPLE"群%s发来一条消息,请到消息盒子处理\n"WHITE,mes.to);
            add_mhead(mes);
        //不是当前聊天的用户就放进消息盒子
    }
}

void get_one_chat(MES mes)
{
    if(strcmp(mes.from,to_who) == 0){
        //如果是当前用户则直接输出
        printf("\t\t%s",mes.detail);
        printf(YELLOW" <-%s"WHITE,mes.from);
        puts("");
    }else{
        if(mes.resault == -1) {
            puts(RED"对方不在线!"WHITE);
        } else{
            printf(PURPLE"%s发来一条消息,请到消息盒子处理\n"WHITE,mes.from);
            add_mhead(mes);
        }
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
            get_group_chat(mes);
			//群聊消息
			break;
		case 5:
			//附近的人
	        List_Add(mes);
			break;
		case 6:
			//我的好友
            if(mes.resault == -1) {
                puts(GREEN"你没有好友,快去附近的人添加吧!"WHITE);
            }
			List_Add(mes);
			break;
		case 7:
			//我的群
            if(mes.resault == -1) {
                puts(GREEN"你没有群,快去创建或者让好友邀请你吧!"WHITE);
            }
			List_Add(mes);
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
            } else if(mes.resault == -2) {
                printf(RED"%s已是你的好友,无需再次添加\n"WHITE,mes.to);
            }
            sleep(1);
			break;
		case 9:
			//加群
            if(mes.resault==-1) {
                printf(BLUE"\t%s邀请你加入群%s,请到消息盒子处理\n"WHITE,mes.from,mes.detail);
                add_mhead(mes);
            } else if(mes.resault == 0) {
                printf(RED"%s拒绝进入群%s\n"WHITE,mes.to,mes.detail);
            } else if(mes.resault == 1) {
                printf(RED"%s同意进入群%s\n"WHITE,mes.to,mes.detail);
            } else if(mes.resault == -2) {
                printf(RED"%s已是该群成员,无需重复邀请\n"WHITE,mes.to);
            } else if(mes.resault == -3) {
                printf(RED"对方不在线,无法接收你的邀请.");
            }
            sleep(1);
			break;
        case 10:
            //退出登陆
            online = 0;
            pthread_exit(0);
            break;
        case 11:
            //私聊记录
            printf(BLUE"%s:"WHITE" %s\n",mes.from,mes.detail);
            break;
        case 12:
            //群聊记录
            printf(BLUE"%s:"WHITE" %s\n",mes.from,mes.detail);
            break;
        case 13:
            //删好友
            printf(RED"%s已经删除了你...\n"WHITE,mes.from);
            break;
        case 18:
            List_Add(mes);
            break;
        }
	}
}

void tishi_qun(void)
{
    puts(YELLOW"-------------------------------");
    puts("-                             -");
    puts("-   输入消息,按回车键发送     -");
    puts("-   -quit-   :返回上一层      -");
    puts("-   -history-:聊天记录        -");
    puts("-   -exit-   :退群            -");
    puts("-   -l.d-    :查看或删除成员  -");
    puts("-   -add-    :邀请成员        -");
    puts("-------------------------------"WHITE);
}

void exit_qun(void)
{
    //退群/解散
    MES mes;
    mes.mode = 17;
    strcpy(mes.from,i_am);
    strcpy(mes.to,qun_name);
    if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
        my_err("send",__LINE__);
        exit(0);
    }
    printf("你已经退出群%s,若你创建了该群则群已被解散.\n",mes.to);
    strcpy(to_who,"NONE");
}

void qun_chat(MES *temp)
{
    tishi_qun();
    //将群名保存到全局
    strcpy(qun_name,to_who);

    MES mes;
    while(temp!=NULL) {
        printf("\t\t%s",temp->detail);
        printf(YELLOW" <-%s"WHITE,temp->from);
        puts("");
        temp=temp->sam_next;
    }
    while(1) {
        memset(&mes,0,sizeof(MES));
        fgets(mes.detail,sizeof(mes.detail),stdin);
        mes.detail[strlen(mes.detail)-1] = '\0';
        if(strcmp(mes.detail,"-quit-") == 0) {
            strcpy(to_who,"NONE");
            break;
        } else if(strcmp(mes.detail,"-history-") == 0) {
            strcpy(mes.from,i_am);
            strcpy(mes.to,to_who);
            mes.mode=12;
            system("printf \"\ec\"");
            puts(RED"聊天记录"WHITE);
            if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
                my_err("send",__LINE__);
                exit(0);
            }
            sleep(1);
            puts("按回车键返回聊天");
            getchar();
            system("printf \"\ec\"");
            tishi_qun();
            continue;
        } else if(strcmp(mes.detail,"-exit-") == 0) {
            exit_qun();
            sleep(1);
            break;
        } else if(strcmp(mes.detail,"-add-") == 0) {
            system("printf \"\ec\"");
            add_group(qun_name);
            tishi_qun();
            continue;
        } else if(strcmp(mes.detail,"-l.d-") == 0) {
            list_all(18);
            system("printf \"\ec\"");
            tishi_qun();
            continue;
        }
        mes.mode = 4;
        strcpy(mes.from,i_am);
        strcpy(mes.to,qun_name);
        if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
            my_err("send",__LINE__);
            exit(0);
        }
    }
}

void ti_ren(void)
{
    MES mes;
    mes.mode=16;
    strcpy(mes.from,i_am);
    strcpy(mes.to,to_who);
    strcpy(mes.detail,qun_name);
    if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
        my_err("send",__LINE__);
        exit(0);
    }
    printf(RED"已删除群成员%s"WHITE,mes.to);
}

void chat(MES *temp)
{
    MES mes;
    puts(YELLOW"---------------------------------");
    puts("-                             -");
    puts("-   输入消息,按回车键发送     -");
    puts("-   -del-    :删除好友        -");
    puts("-   -quit-   :返回上一层      -");
    puts("-   -history-:聊天记录        -");
    puts("-------------------------------"WHITE);
    while(temp!=NULL) {
        printf("\t\t%s",temp->detail);
        printf(YELLOW" <-%s"WHITE,temp->from);
        puts("");
        temp=temp->sam_next;
    }
    while(1) {
        memset(&mes,0,sizeof(MES));
        fgets(mes.detail,sizeof(mes.detail),stdin);
        mes.detail[strlen(mes.detail)-1] = '\0';
        if(strcmp(mes.detail,"-quit-") == 0) {
            strcpy(to_who,"NONE");
            break;
        } else if(strcmp(mes.detail,"-history-") == 0) {
            strcpy(mes.from,i_am);
            strcpy(mes.to,to_who);
            mes.mode=11;
            system("printf \"\ec\"");
            puts(RED"聊天记录"WHITE);
            if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
                my_err("send",__LINE__);
                exit(0);
            }
            sleep(1);
            puts("按回车键返回聊天");
            getchar();
            system("printf \"\ec\"");
            puts(YELLOW"---------------------------------");
            puts("-                             -");
            puts("-   输入消息,按回车键发送     -");
            puts("-   -del-    :删除好友        -");
            puts("-   -quit-   :返回上一层      -");
            puts("-   -history-:聊天记录        -");
            puts("-------------------------------"WHITE);
            continue;
        } else if(strcmp(mes.detail,"-del-") == 0) {
            mes.mode = 13;
            strcpy(mes.from,i_am);
            strcpy(mes.to,to_who);
            if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
                my_err("send",__LINE__);
                exit(0);
            }
            puts(RED"已删除,按回车键返回..."WHITE);
            sleep(1);
            getchar();
            strcpy(to_who,"NONE");
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
    if(mes.resault == -1) {
        return;
    }
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
    puts("进入list_view");
    U_L *temp;
    if(head->next == NULL) {
        puts("0个好友!!");
        return 0;
    }
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
            printf("[私聊]%s[%d条]\n",temp->from,temp->num);
            break;
        case 4:
            printf("[群聊]%s[%d条]\n",temp->to,temp->num);
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
            if(temp->resault == -1) {
                printf("%s邀请你加入群%s\n",temp->from,temp->detail);
            } else if(temp->resault == 0) {
                printf("拒绝进群\n");
            } else if(temp->resault == 1) {
                printf("同意加群\n");
            }
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
    //删除被处理的消息列表
    MES *tt=mhead;
    while(tt->next!=temp) {
        tt=tt->next;
    }
    tt->next=temp->next;
    switch(temp->mode) {
    case 3:
        strcpy(to_who,temp->from);
        chat(temp);
        break;
    case 4:
        printf("[群聊]%s\n",temp->to);
        qun_chat(temp);
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
        if(temp->resault==-1) {
            printf("%s邀请你加入群%s,是否同意(y/n)?\n",temp->from,temp->detail);
            char ch;
            do{
                printf("请输入:");
                ch = getchar();
            }while(ch!='y'&&ch!='n');
            MES mes;
            memcpy(&mes,temp,sizeof(MES));
            if(ch=='y') {
                puts("接收入群邀请!");
                mes.resault=1;
            } else {
            puts("拒绝入群邀请!");
            mes.resault=0;
            }
            if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
                my_err("send",__LINE__);
                exit(0);
            }
        }else if(temp->resault == 0) {
            printf("%s拒绝了你的邀请\n",temp->to);
        }else if(temp->resault == 1) {
            printf("%s同意了你的邀请\n",temp->to);
        }
        sleep(1);
        break;
    case 10:
        break;
    case 11:
        break;
    case 12:
        break;
    }
    free(temp);

}

void logout(void)
{
    MES mes;
    mes.mode = 10;
    strcpy(mes.from,i_am);
    if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
        my_err("send",__LINE__);
        exit(0);
    }
    puts(BLUE"正在像服务器发送下线请求..."WHITE);
}

void change_pass(void)
{
    MES mes;
    system("printf \"\ec\"");
    puts(RED"\t注意!你现在在修改密码,请谨慎操作!"WHITE);
    puts(GREEN"\n\t若你后悔进入此项功能只需两次输入不相同即可退出!"WHITE);
    char pass[20];
    char passp[20];
    memcpy(pass,getpass("请输入密码:"),20);
    setbuf(stdin,NULL);
    memcpy(passp,getpass("请再次输入:"),20);
    setbuf(stdin,NULL);
    if(strcmp(pass,passp) == 0) {
        memset(&mes,0,sizeof(mes));
        strcpy(mes.detail,pass);
        mes.mode = 14;
        strcpy(mes.from,i_am);
        if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
            my_err("send",__LINE__);
            exit(0);
        }
        puts(PURPLE"已修改密码!下次登陆请使用新密码!"WHITE);
    } else {
        puts(PURPLE"两次密码不匹配!"WHITE);
    }
    sleep(1);
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
    puts("5.修改密码");
    puts("6.创建群");
    puts("7.退出登陆"WHITE);
    puts("");
    int sel;
    do{
        sel=Sel();
    }while(sel<1||sel>7);
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
        list_all(7);
        Sign_Ok();
        break;
    case 4:
        mess_box();
        Sign_Ok();
        break;
    case 5:
        change_pass();
        Sign_Ok();
        break;
    case 6:
        //建群
        creat_group();
        Sign_Ok();
        break;
    case 7:
        logout();
        sleep(1);
        if(online == 0) {
            puts("成功下线");
            exit(0);
        } else {
            puts("下线失败...");
            Sign_Ok();
        }
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
    strcpy(to_who,"NONE");
    sleep(1);
}

int add_group(char *name)
{
    puts("邀群友!");
    char nn[20];
    strcpy(nn,name);
    head = (U_L*)malloc(sizeof(U_L));
    head->next=NULL;
    MES mes;
    memset(&mes,0,sizeof(MES));
    mes.mode = 6;
    strcpy(mes.from,i_am);
    if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
        //给服务器发送需要获取用户列表的的信息
        my_err("send",__LINE__);
        exit(0);
    }
    sleep(1);
    int max = List_View();
    if(max == 0) return 0;
    int sel;
    puts(RED"请输入序号来邀请,输入0返回上一层"WHITE);
    do{
        sel = Sel();
    } while(sel < 0 || sel > max);
    if(sel == 0) return 0;
    Search_List(sel);//检查所选编号所对应的用户名
    destory();
    memset(&mes,0,sizeof(MES));
    mes.mode=9;
    mes.resault = -1;
    strcpy(mes.from,i_am);
    strcpy(mes.to,to_who);
    strcpy(mes.detail,nn);
    if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
        my_err("send",__LINE__);
        exit(0);
    }
    puts(GREEN"邀请已发出,请耐心等候..."WHITE);
    sleep(1);
    strcpy(to_who,nn);
    return 1;
}

void creat_group(void)
{
    MES mes;
    memset(&mes,0,sizeof(MES));
    strcpy(mes.from,i_am);
    mes.mode = 15;
    printf("请输入群名称:");
    char name[20];
    scanf("%s",name);
    strcpy(mes.to,name);
    if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
        my_err("send",__LINE__);
        exit(0);
    }
    puts(RED"建群成功!"WHITE);
    sleep(1);
    //邀请好友入群
    while(1){
    if(!add_group(name))
        break;
    }
}

int list_all(int n)
{
    head = (U_L*)malloc(sizeof(U_L));
    head->next=NULL;
    MES mes;
    memset(&mes,0,sizeof(MES));
    mes.mode = n;
    strcpy(mes.from,i_am);
    if(n == 18) {
        strcpy(mes.to,qun_name);
    }
    if(send(ser_fd,&mes,sizeof(MES),0) < 0) {
        //给服务器发送需要获取用户列表的的信息
        my_err("send",__LINE__);
        exit(0);
    }
    sleep(1);
    int max = List_View();
    if(n == 18) {
        U_L *hh=head->next;
        while(hh->next!=NULL) {
            hh=hh->next;
        }
        if(strcmp(hh->name,i_am) != 0) {
            puts("你没有权限踢人,请按回车返回聊天!");
            getchar();
            return 0;
        }
    }
    if(max == 0) return 0;
    int sel;
    if(n == 5) {
        puts(RED"请输入序号来添加好友,输入0返回上一层"WHITE);    
    } else if(n == 6) {
        puts(RED"请输入序号来选择好友聊天,输入0返回上一层"WHITE);
    } else if(n == 7) {
        puts(RED"请输入序号来进行群聊,输入0返回上一层,"WHITE);
    } else if(n==18) {
        puts(RED"输入序号删除,输入0返回上一层"WHITE);
    }
    do{
        sel = Sel();
    } while(sel < 0 || sel > max);
    if(sel == 0) return 0;
    Search_List(sel);//检查所选编号所对应的用户名
    destory();
    if(strcmp(to_who,i_am) == 0) {
        puts(RED"不能选择自己"WHITE);
        puts(qun_name);
        sleep(1);
        return 0;
    }
    if(n == 5) {
        add_friend();    
    } else if(n == 6) {
        chat(NULL);
    } else if(n == 7) {
        qun_chat(NULL);
    } else if(n == 18) {
        ti_ren();
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
    MES *temp;//复制消息的指针
    MES *tp;//查重指针
    int pan = 0;
    temp=(MES*)malloc(sizeof(MES));
    memcpy(temp,&mes,sizeof(MES));
    temp->sam_next = NULL;
    temp->num=1;
    
    tp=mhead->next;
    while(tp != NULL) {
        if(tp->mode == temp->mode&&strcmp(tp->from,temp->from) == 0) {
            tp->num++;
            MES *t1,*t2;
            t1=tp;
            while(t1->sam_next!=NULL) {
                t1=t1->sam_next;
            }
            temp->sam_next=t1->sam_next;
            t1->sam_next=temp;
            pan = 1;
            puts("合并一条消息");
            break;
        }
    }
    if(!pan) {
        puts("添加一条消息");
        temp->next=mhead->next;
        mhead->next=temp;
    }
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
