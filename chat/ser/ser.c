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

#define Sign_In             1 //登陆
#define Sign_Up             2 //注册
#define Chat_One            3 //私聊
#define Chat_Group          4 //群聊
#define List_All            5 //周围的人
#define List_My             6 //我的好友
#define List_group          7 //我的群
#define Add_friend          8 //添加好友
#define Add_group           9 //加群
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

U_L *head;

void chat_one(MES mes);
void add_list(char *name,int fd);
void creat_list(void);
void my_err(const char *err_string,int line);
int match(int mode,char *ch,int conn_fd);
int search_fd(char *name);
void chat_one(MES mes);
void list_all(int conn_fd);
void Sign_Ok(int conn_fd);
void sign_in_up(int conn_fd);
void add_friend(MES mes);
void list_friend(int conn_fd);
char *search_name(int conn_fd);


void add_list(char *name,int fd)
{
    printf("%d\n",fd);
    U_L *temp;
    temp = (U_L*)malloc(sizeof(U_L));
    strcpy(temp->name,name);
    temp->num=fd;
    temp->next=head->next;
    head->next=temp;
}
void creat_list(void)
{
    //将用户名和fd绑定起来
    head = (U_L*)malloc(sizeof(U_L));
    head -> next = NULL;
}

void my_err(const char *err_string,int line)
{
    fprintf(stderr,"line:%d",line);
    perror(err_string);
    exit(1);
}

int create_friend_table(char *name)
{
    //注册成功创建好友表
    char sql[50];
    sprintf(sql,"CREATE TABLE friend_%s (name VARCHAR(20),mode INT);",name);
    puts(sql);
    MYSQL *conn;
    conn = mysql_init(NULL);
    if(conn == NULL) { 
        //如果返回NULL说明初始化失败
        printf("mysql_init failed!\n");
        return EXIT_FAILURE;
    }
    conn = mysql_real_connect(conn,"localhost","lyt","","chat",0,NULL,0);
    if (!conn) {
        printf("Connection failed!\n");
    }
    int res;
    res = mysql_query(conn,sql);//正确返回0
    if(res) {
        perror("my_query");
        mysql_close(conn);
        exit(0);
    }
    mysql_close(conn);
}

int match(int mode,char *ch,int conn_fd)
{
    //匹配用户输入的用户名和密码
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
            create_friend_table(name);
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

char *search_name(int conn_fd)
{
	//根据fd查询用户名
    U_L *temp;
    temp = head->next;
    while(temp != NULL) {
        if(temp->num==conn_fd) {
            return temp->name;
        }
        temp=temp->next;
    }
    return NULL;    
}

int search_fd(char *name)
{
    //查找用户名对应的FD,找不到返回0
    U_L *temp;
    temp = head->next;
    while(temp != NULL) {
        if(strcmp(name,temp->name) == 0) {
            return temp->num;
        }
        temp=temp->next;
    }
    return 0;
}

void chat_one(MES mes)
{
    //私聊转发消息
    int fd;
    fd = search_fd(mes.to);
    if(send(fd,&mes,sizeof(MES),0) < 0) {
        my_err("send",__LINE__);
        exit(0);
    } 
}

void list_all(int conn_fd)
{
    //将所有在线用户发送给客户端
    U_L *temp;
    MES mes;
    temp= head->next;
    while(temp!=NULL) {
        memset(&mes,0,sizeof(MES));
        strcpy(mes.detail,temp->name);
        puts(temp->name);
        mes.mode = 5;
        mes.resault = 1;
        if(send(conn_fd,&mes,sizeof(MES),0) < 0) {
            my_err("send",__LINE__);
            exit(0);
        }
        temp=temp->next;
    }
}

void insert_friend(MES mes)
{
    //将成功加到的好友添加进入数据表
    char sql[50];
    sprintf(sql,"INSERT INTO friend_%s VALUES('%s',0);",mes.to,mes.from);
    puts(sql);
    MYSQL *conn;
    conn = mysql_init(NULL);
    if(conn == NULL) { 
        //如果返回NULL说明初始化失败
        printf("mysql_init failed!\n");
        return EXIT_FAILURE;
    }
    conn = mysql_real_connect(conn,"localhost","lyt","","chat",0,NULL,0);
    if (!conn) {
        printf("Connection failed!\n");
    }
    int res;
    res = mysql_query(conn,sql);//正确返回0
    if(res) {
        perror("my_query");
        mysql_close(conn);
        exit(0);
    }
    sprintf(sql,"INSERT INTO friend_%s VALUES('%s',0);",mes.from,mes.to);
    res = mysql_query(conn,sql);//正确返回0
    if(res) {
        perror("my_query");
        mysql_close(conn);
        exit(0);
    } 
}

void add_friend(MES mes)
{
    //添加好友
    int fd;
    if(mes.resault == -1) {
        fd=search_fd(mes.to);
    } else {
        fd=search_fd(mes.from);
    }
    if(mes.resault==1) {
        //对方同意添加好友
        insert_friend(mes);
    }
    if(send(fd,&mes,sizeof(MES),0) < 0) {
        my_err("send",__LINE__);
        exit(0);
    }
}

void list_friend(int conn_fd)
{
    char sql[50];
    sprintf(sql,"SELECT DISTINCT name FROM friend_%s WHERE mode=0;",search_name(conn_fd));
    int res;//执行sql语句后的返回标志
    MYSQL_RES *res_ptr;//指向查询结果的指针
    MYSQL_FIELD *field;//字段结构指针
    MYSQL_ROW result_row;//按行返回查询信息
    int row,column;//查询返回的行数和列数
    MYSQL *conn;//一个数据库链接
    int i,j;

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
    if (conn) {
        printf("Connection success!\n");
    } else {
        printf("Connection failed!\n");
    }
    mysql_query(conn,"set names gbk");//防止乱码。设置和数据库的编码一致就不会乱码

    MES mes;
    res = mysql_query(conn,sql);//正确返回0
    if(res) {
        perror("mysql_query");
        mysql_close(conn);
        exit(0);
    } else{
        //把查询结果给res_ptr
        res_ptr = mysql_store_result(conn);
        //如果结果不为空,则输出
        if(res_ptr) {
            column = mysql_num_fields(res_ptr);
            row = mysql_num_rows(res_ptr);
            //printf("查到%d行,%d列\n",row,column);
            //输出结果的字段名
            /*
            for(i = 0;field = mysql_fetch_field(res_ptr);i++) {
                printf("%10s",field->name);
            }
            puts("");
            */
            //按行输出结果
            for(i = 1;i < row+1;i++){
                result_row = mysql_fetch_row(res_ptr);
                for(j = 0;j< column;j++) {
                    memset(&mes,0,sizeof(MES));
                    strcpy(mes.detail,result_row[j]);
                    mes.mode = 6;
                    if(send(conn_fd,&mes,sizeof(MES),0) < 0) {
                        my_err("send",__LINE__);
                        exit(0);
                    }
                    
                    printf("%10s",result_row[j]);
                }
                //puts("");
            }
        }
    }
    //退出前关闭连接
    mysql_close(conn);
}

void Sign_Ok(int conn_fd)
{
    //登陆成功后开始不断接收消息
    MES mes;
    while(1){
        memset(&mes,0,sizeof(MES));
        if(recv(conn_fd,&mes,sizeof(MES),0) < 0) {
            my_err("recv",__LINE__);
            exit(0);
        }
        switch(mes.mode){
        case 3:
            //私聊
            chat_one(mes);
            break;
            ;
        case 4:
            //群聊
            //chat_group(mes);
            ;
        case 5:
            //所有人
            printf("%d 请求查看所有在线用户\n",conn_fd);
            list_all(conn_fd);
            break;
        case 6:
            //好友
            printf("%d 请求查看好友列表\n",conn_fd);
            list_friend(conn_fd);
            break;
        case 8:
            //添加好友
            printf("%d 请求添加好友\n",conn_fd);
            add_friend(mes);
            break;
        }
        
    }
}

void sign_in_up(int conn_fd)
{
    //接收登陆用户名密码
while(1) {
    MES mes;
    int re;
    memset(&mes,0,sizeof(MES));
    if(re = recv(conn_fd,&mes,sizeof(MES),0) <= 0) {
        if(re < 0) {
        my_err("recv",__LINE__);
        exit(0);
        }
        if(re == 0) {
            close(conn_fd);
            exit(0);
        }
    }
    int res;
    res = match(mes.mode,mes.detail,conn_fd);
    char name[20];
    int i;
    for(i =0;mes.detail[i]!=';';i++) {
        name[i]=mes.detail[i];
    }
    name[i] = '\0';
    if(res) {
        puts("res");
        //跳转到登陆或聊天页面
        add_list(name,conn_fd);
        printf("%s已登陆\n",name);
/*
        pthread_t thid;
        pthread_create(&thid,NULL,(void*)my_recv,(void*)conn_fd);
*/
        Sign_Ok(conn_fd);
    }
}
}
int main()
{
    creat_list();
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
        printf("一个新的连接,来自 %s\n",inet_ntoa(cli_addr.sin_addr));

        pthread_t thid;
        pthread_create(&thid,NULL,(void*)sign_in_up,(void*)conn_fd);
    }
}
