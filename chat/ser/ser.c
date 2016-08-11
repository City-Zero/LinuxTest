/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: 2016年08月09日 星期二 11时05分52秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>
#include<time.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<mysql/mysql.h>
#include<string.h>
#include<pthread.h>

#define SERV_PORT   4040
#define LISTENQ     5

#define USERNAME    0
#define PASSWORD    1

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

U_L *head;

void add_list(char *name,int fd)
{
    U_L *temp;
    temp = (U_L*)malloc(sizeof(U_L));
    strcpy(temp->name,name);
    temp->num=fd;
    temp->next=head->next;
    head->next=temp;
}
void creat_list(void)
{
    head = (U_L*)malloc(sizeof(U_L));
    head -> next = NULL;
}

void my_err(const char *err_string,int line)
{
    fprintf(stderr,"line:%d",line);
    perror(err_string);
    exit(1);
}

int match(int mode,char *ch,int conn_fd)
{
    MES mes;
    memset(&mes,0,sizeof(MES));
    char sql[50];
    int res;//执行sql语句后的返回标志
    MYSQL_RES *res_ptr;//指向查询结果的指针
    MYSQL_FIELD *field;//字段结构指针
    MYSQL_ROW result_row;//按行返回查询信息
    int row,column;//查询返回的行数和列数
    MYSQL *conn;//一个数据库链接
    int i,j;
    int pan=0;
    char name[20],pass[20];
    for(i=0;ch[i]!=';';i++) {
        name[i] = ch[i];
    }
    name[i] = '\0';
    i++;
    for(j=0;ch[i]!='\0';j++,i++) {
        pass[j]=ch[i];
    }
    pass[j]='\0';
    puts(name);
    puts(pass);
    //初始化连接句柄
    conn = mysql_init(NULL);

    if(conn == NULL) { 
        //如果返回NULL说明初始化失败
        printf("mysql_init failed!\n");
        return EXIT_FAILURE;
    }

    //进行实际连接
    //参数　conn连接句柄，host　mysql所在的主机或地址,user用户名,passwd密码,database_name数据库名,后面的都是默认
    conn = mysql_real_connect(conn,"localhost","lyt","","chat",0,NULL,0);
    if (!conn) {
        printf("Connection failed!\n");
    }
    mysql_query(conn,"set names gbk");//防止乱码。设置和数据库的编码一致就不会乱码

    if(mode == 2) {
        sprintf(sql,"SELECT name FROM info WHERE name='%s';",name);
        puts(sql);
        //查询注册是否重名
        res = mysql_query(conn,sql);//正确返回0
        if(res) {
            perror("my_query");
            mysql_close(conn);
            exit(0);
        } else{
            //把查询结果给res_ptr
            res_ptr = mysql_store_result(conn);
            //如果结果为空,未重名,添加账号
            if(res_ptr) {
                int ro;
                ro = mysql_num_rows(res_ptr);
                if(ro == 0){
                    pan = 1;
                } 
            }
        }
		if(pan) {
            sprintf(sql,"INSERT INTO info(name,pass) VALUES('%s','%s');",name,pass);
            puts(sql);
            res = mysql_query(conn,sql);//正确返回0
            if(res) {
                perror("my_query");
                mysql_close(conn);
                exit(0);
            }
            mes.resault = 1;
            if(send(conn_fd,&mes,sizeof(MES),0) < 0) {
                my_err("send",__LINE__);
                exit(0);
            }
        }else {
            mes.resault = 0;
            if(send(conn_fd,&mes,sizeof(MES),0) < 0) {
                my_err("send",__LINE__);
                exit(0);
            } 
        }
    } else if(mode == 1) {
        sprintf(sql,"SELECT * FROM info WHERE name='%s' AND pass='%s';",name,pass);
        //查询登陆是否正确
    	res = mysql_query(conn,sql);//正确返回0
    	if(res) {
        	perror("my_query");
        	mysql_close(conn);
        	exit(0);
    	} else{
        	//把查询结果给res_ptr
        	res_ptr = mysql_store_result(conn);
        	//如果结果不为空,则登陆成功
            if(res_ptr) {
                int ro;
                ro = mysql_num_rows(res_ptr);
                if(ro != 0){
                    pan = 1;
                } 
            }
        }
		if(pan) {
            mes.resault = 1;
            if(send(conn_fd,&mes,sizeof(MES),0) < 0) {
                my_err("send",__LINE__);
                exit(0);
            }
        }else {
            mes.resault = 0;
            if(send(conn_fd,&mes,sizeof(MES),0) < 0) {
                my_err("send",__LINE__);
                exit(0);
            } 
        }
    }
    //退出前关闭连接
    mysql_close(conn);
    return pan;
}

void sign_in_up(int conn_fd)
{
while(1) {
    MES mes;
    memset(&mes,0,sizeof(MES));
    if(recv(conn_fd,&mes,sizeof(MES),0) < 0) {
        my_err("recv",__LINE__);
        exit(0);
    }
    int res;
    printf("%d,%s\n",mes.mode,mes.detail);
    res = match(mes.mode,mes.detail,conn_fd);
    char name[20];
    int i;
    for(i =0;mes.detail[i]!=';';i++) {
        name[i]=mes.detail[i];
    }
    name[i] = '\0';
    puts("name");
    if(res) {
        //跳转到登陆或聊天页面
        add_list(name,conn_fd);
    }
}
}
int main()
{
    int sock_fd,conn_fd;
    int optval;
    int flag_recv;
    int ret;
    int name_num;
    socklen_t cli_len;
    struct sockaddr_in cli_addr,serv_addr;
    char recv_buf[255];

    sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if(sock_fd < 0) {
        my_err("socket",__LINE__);
    }

    optval = 1;

    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(void*)&optval,sizeof(int) ) < 0) {
        my_err("setsockopt",__LINE__);
    }

    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr_in)) < 0) {
        my_err("bind",__LINE__);
    }

    if(listen(sock_fd,LISTENQ) <0 ) {
        my_err("listen",__LINE__);
    }

    cli_len = sizeof(struct sockaddr_in);
    while(1)
    {
        conn_fd = accept(sock_fd,(struct sockaddr*)&cli_addr,&cli_len);
        if(conn_fd < 0) {
            my_err("accept",__LINE__);
        }
        printf("一个新的连接,来自 %s,conn_fd = %d\n",inet_ntoa(cli_addr.sin_addr),conn_fd);

        pthread_t thid;
        pthread_create(&thid,NULL,(void*)sign_in_up,(void*)conn_fd);
    }
}
