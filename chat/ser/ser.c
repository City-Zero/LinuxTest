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
#define LogOut              10//退出登陆
#define One_History         11//私聊记录
#define Gro_History         12//群聊记录
#define Del_friend          13//删除好友
#define Cha_pass            14//修改密码
#define Cre_group           15//建群
#define Del_user            16//群主踢人
#define Exit_group          17//退群或解散群
#define Group_user          18//查看群成员
#define Sys_mes             19//服务器消息


typedef struct message
{
    int mode;//模式
    int resault;//状态
    char to[20];
    char from[20];
    char detail[255];
    int mun;
    struct message *next;
    struct message *sam_next;

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
void list_friend(int conn_fd,int mode,char *qunming);
char *search_name(int conn_fd);
void del(int conn_fd);
void logout(int conn_fd);
void add_one_message(MES mes);
void del_frined(MES mes);
int compare_group(MES mes);
void creat_group(MES mes);
void add_group(MES mes);
void insert_group(MES mes);
void exit_group(MES mes);
void del_group_user(MES mes);
void add_group_message(MES mes);
void chat_group(MES mes);


void add_list(char *name,int fd)
{
    //printf("%d\n",fd);
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
    //puts(sql);
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
        //puts(sql);
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
            //puts(sql);
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
                    U_L *temp=head->next;
                    while(temp!=NULL) {
                        if(strcmp(temp->name,name) == 0) {
                            pan = 0;
                            break;
                        }
                        temp=temp->next;
                    }
                    if(temp==NULL) pan = 1;
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

void add_group_message(MES mes)
{
    char ttime[30];
    time_t timep;
    struct tm *p;
    time(&timep);
    p=localtime(&timep);
    //获取当地时间
    sprintf(ttime,"%d.%d.%d %d:%d:%d",(1900+p->tm_year),(1+p->tm_mon), p->tm_mday,p->tm_hour, p->tm_min, p->tm_sec);
    char sql[255];
    sprintf(sql,"INSERT INTO history_group_%s VALUES('%s','%s','%s');",mes.to,mes.detail,mes.from,ttime);
    //puts(sql);
    MYSQL *conn;
    conn = mysql_init(NULL);
    if(conn == NULL) { 
        //如果返回NULL说明初始化失败
        printf("mysql_init failed!\n");
        return;// EXIT_FAILURE;
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
    puts("保存好了");
    mysql_close(conn);
    puts("关闭mysql");
}


void add_one_message(MES mes)
{
    char ttime[30];
    time_t timep;
    struct tm *p;
    time(&timep);
    p=localtime(&timep);
    //获取当地时间
    sprintf(ttime,"%d.%d.%d %d:%d:%d",(1900+p->tm_year),(1+p->tm_mon), p->tm_mday,p->tm_hour, p->tm_min, p->tm_sec);
    char sql[255];
    if(strcmp(mes.from,mes.to) < 0) {
    sprintf(sql,"INSERT INTO message_%s_%s VALUES('%s','%s','%s','%s');",mes.from,mes.to,mes.detail,mes.from,mes.to,ttime);
    } else {
    sprintf(sql,"INSERT INTO message_%s_%s VALUES('%s','%s','%s','%s');",mes.to,mes.from,mes.detail,mes.from,mes.to,ttime);
    }
    //puts(sql);
    MYSQL *conn;
    conn = mysql_init(NULL);
    if(conn == NULL) { 
        //如果返回NULL说明初始化失败
        printf("mysql_init failed!\n");
        return;// EXIT_FAILURE;
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
    //puts("保存好了");
    mysql_close(conn);
    //puts("关闭mysql");
}

void chat_group(MES mes)
{
    char sql[255];
    int res;//执行sql语句后的返回标志
    MYSQL_RES *res_ptr;//指向查询结果的指针
    MYSQL_FIELD *field;//字段结构指针
    MYSQL_ROW result_row;//按行返回查询信息
    int row,column;//查询返回的行数和列数
    MYSQL *conn;//一个数据库链接
    //初始化连接句柄
    conn = mysql_init(NULL);

    if(conn == NULL) { 
        //如果返回NULL说明初始化失败
        printf("mysql_init failed!\n");
        return;// EXIT_FAILURE;
    }

    conn = mysql_real_connect(conn,"localhost","lyt","","chat",0,NULL,0);
    if (!conn) {
        printf("Connection failed!\n");
    }
    mysql_query(conn,"set names gbk");
    //防止乱码。设置和数据库的编码一致就不会乱码
	sprintf(sql,"SELECT * FROM group_%s;",mes.to);
    //puts(sql);

    res = mysql_query(conn,sql);//正确返回0
    if(res) {
        perror("my_query");
        mysql_close(conn);
        exit(0);
    } else{
        //把查询结果给res_ptr
        res_ptr = mysql_store_result(conn);
        if(res_ptr) {
           int ro;
            ro = mysql_num_rows(res_ptr);
            int i;
            for(i=1;i<ro+1;i++) {
				//将群消息发给每个人
                result_row = mysql_fetch_row(res_ptr);
                //printf("打算发消息给%s\n",result_row[0]);
				int dd;
				if((dd=search_fd(result_row[0])) == 0) {
					continue;
				}
				if(search_fd(result_row[0]) == search_fd(mes.from)) {
					continue;
				}
				if(send(dd,&mes,sizeof(MES),0) < 0) {
        			my_err("send",__LINE__);
       				exit(0);
    			}
                //printf("已经发消息给%s\n",result_row[0]);
			}
       		add_group_message(mes);
			//puts("添加一条群聊记录!");
        }
    }
    mysql_close(conn);
}


void chat_one(MES mes)
{
    //私聊转发消息
    int fd;
    fd = search_fd(mes.to);
    if(fd == 0) {
        fd = search_fd(mes.from);
        mes.resault = -1;
    }
    if(send(fd,&mes,sizeof(MES),0) < 0) {
        my_err("send",__LINE__);
        exit(0);
    }
    if(mes.resault != -1) {
        add_one_message(mes);
    }
    //puts("保存一条记录");
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
        //puts(temp->name);
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
    char sql[255];
    sprintf(sql,"INSERT INTO friend_%s VALUES('%s',0);",mes.to,mes.from);
    //puts(sql);
    MYSQL *conn;
    conn = mysql_init(NULL);
    if(conn == NULL) { 
        //如果返回NULL说明初始化失败
        printf("mysql_init failed!\n");
        return;// EXIT_FAILURE;
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
    if(strcmp(mes.from,mes.to) < 0) {
        sprintf(sql,"CREATE TABLE message_%s_%s \
    ( mes VARCHAR(255), send VARCHAR(20),\
    recv VARCHAR(20), time VARCHAR(30));",mes.from,mes.to);
    } else{
        sprintf(sql,"CREATE TABLE message_%s_%s \
    ( mes VARCHAR(255), send VARCHAR(20),\
    recv VARCHAR(20), time VARCHAR(30));",mes.to,mes.from); 
    }
    //puts(sql);
    mysql_query(conn,sql);

    mysql_close(conn);
}

int compare_friend(MES mes)
{
    char sql[50];
    int res;//执行sql语句后的返回标志
    MYSQL_RES *res_ptr;//指向查询结果的指针
    MYSQL_FIELD *field;//字段结构指针
    MYSQL_ROW result_row;//按行返回查询信息
    int row,column;//查询返回的行数和列数
    MYSQL *conn;//一个数据库链接
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

    sprintf(sql,"SELECT name FROM friend_%s WHERE name='%s';",mes.from,mes.to);
    //puts(sql);
    //查询注册是否重名
    res = mysql_query(conn,sql);//正确返回0
    if(res) {
        perror("my_query");
        mysql_close(conn);
        exit(0);
    } else{
        //把查询结果给res_ptr
        res_ptr = mysql_store_result(conn);
        //如果结果为空,没有这个好友,添加
        if(res_ptr) {
           int ro;
            ro = mysql_num_rows(res_ptr);
            if(ro == 0){
                return 1;
            } else {
				return 0;
			}
        }
    }
}

int compare_group(MES mes)
{
    char sql[50];
    int res;//执行sql语句后的返回标志
    MYSQL_RES *res_ptr;//指向查询结果的指针
    MYSQL_FIELD *field;//字段结构指针
    MYSQL_ROW result_row;//按行返回查询信息
    int row,column;//查询返回的行数和列数
    MYSQL *conn;//一个数据库链接
    //初始化连接句柄
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
    mysql_query(conn,"set names gbk");//防止乱码。设置和数据库的编码一致就不会乱码

    sprintf(sql,"SELECT name FROM group_%s WHERE name='%s';",mes.detail,mes.to);
    //puts(sql);
    //查询注册是否重名
    res = mysql_query(conn,sql);//正确返回0
    if(res) {
        perror("my_query");
        mysql_close(conn);
        exit(0);
    } else{
        //把查询结果给res_ptr
        res_ptr = mysql_store_result(conn);
        //如果结果为空,没有这个群友,添加
        if(res_ptr) {
           int ro;
            ro = mysql_num_rows(res_ptr);
            if(ro == 0){
                mysql_close(conn);
                return 1;
            } else {
                mysql_close(conn);
				return 0;
			}
        }
    }
}

void creat_group(MES mes)
{
    //创建群组数据库表
    char sql[255];
    int res;//执行sql语句后的返回标志
    MYSQL_RES *res_ptr;//指向查询结果的指针
    MYSQL_FIELD *field;//字段结构指针
    MYSQL_ROW result_row;//按行返回查询信息
    int row,column;//查询返回的行数和列数
    MYSQL *conn;//一个数据库链接
    //初始化连接句柄
    conn = mysql_init(NULL);

    if(conn == NULL) { 
        //如果返回NULL说明初始化失败
        printf("mysql_init failed!\n");
        return;// EXIT_FAILURE;
    }

    conn = mysql_real_connect(conn,"localhost","lyt","","chat",0,NULL,0);
    if (!conn) {
        printf("Connection failed!\n");
    }
    mysql_query(conn,"set names gbk");//防止乱码。设置和数据库的编码一致就不会乱码

    sprintf(sql,"CREATE TABLE group_%s (name VARCHAR(20),mode INT);",mes.to);
    //puts(sql);
    mysql_query(conn,sql);//正确返回0
    sprintf(sql,"INSERT INTO group_%s (name,mode) VALUES('%s',1);",mes.to,mes.from);
    //puts(sql);
    mysql_query(conn,sql);//正确返回0
    sprintf(sql,"INSERT INTO friend_%s (name,mode) VALUES('%s',1);",mes.from,mes.to);
    //puts(sql);
    mysql_query(conn,sql);//正确返回0
    sprintf(sql,"CREATE TABLE history_group_%s (mes VARCHAR(255),send VARCHAR(20),time VARCHAR(30));",mes.to);
    //puts(sql);
    mysql_query(conn,sql);//正确返回0
    mysql_close(conn);
}

void insert_group(MES mes)
{
    char sql[255];
    sprintf(sql,"INSERT INTO friend_%s VALUES('%s',1);",mes.to,mes.detail);
    //puts(sql);
    MYSQL *conn;
    conn = mysql_init(NULL);
    if(conn == NULL) { 
        //如果返回NULL说明初始化失败
        printf("mysql_init failed!\n");
        return;// EXIT_FAILURE;
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
    sprintf(sql,"INSERT INTO group_%s VALUES('%s',0);",mes.detail,mes.to);
    //puts(sql);
    res = mysql_query(conn,sql);//正确返回0
    if(res) {
        perror("my_query");
        mysql_close(conn);
        exit(0);
    }

    mysql_close(conn);
}

void add_group(MES mes)
{
    //添加好友
    int fd;
    if(mes.resault == -1) {
        //-1 邀请加入
		if(compare_group(mes) == 1) {
            //不是群友请求被受理
        	fd=search_fd(mes.to);
		} else {
			mes.resault = -2;
            //已是群友,拒绝请求
            fd = search_fd(mes.from);
    		if(send(fd,&mes,sizeof(MES),0) < 0) {
        		my_err("send",__LINE__);
        		exit(0);
			}
            return;
		}
    } else {
        fd=search_fd(mes.from);
    }
    if(mes.resault==1) {
        //对方同意加入群
        insert_group(mes);
    }
    if(fd == 0) {
        fd = search_fd(mes.from);
        mes.resault = -3;
        //对方不在线
    }
    if(send(fd,&mes,sizeof(MES),0) < 0) {
        my_err("send",__LINE__);
        exit(0);
    }
}

void add_friend(MES mes)
{
    //添加好友
    int fd;
    if(mes.resault == -1) {
        //-1 邀请加入
		if(compare_friend(mes) == 1) {
            //不是好友请求被受理
        	fd=search_fd(mes.to);
		} else {
			mes.resault = -2;
            //已是好友,拒绝请求
            fd = search_fd(mes.from);
    		if(send(fd,&mes,sizeof(MES),0) < 0) {
        		my_err("send",__LINE__);
        		exit(0);
			}
            return;
		}
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

void list_friend(int conn_fd,int mode,char *qunming)
{
    char sql[255];
    if(mode ==6) {
        sprintf(sql,"SELECT name FROM friend_%s WHERE mode=0;",search_name(conn_fd));
    } else if(mode == 7) {
        sprintf(sql,"SELECT DISTINCT name FROM friend_%s WHERE mode=1;",search_name(conn_fd));
    } else if(mode == 18) {
        sprintf(sql,"SELECT DISTINCT name FROM group_%s;",qunming);
    }
    //puts(sql);
    int res;//执行sql语句后的返回标志
    MYSQL_RES *res_ptr;//指向查询结果的指针
    MYSQL_FIELD *field;//字段结构指针
    MYSQL_ROW result_row;//按行返回查询信息
    int row,column;//查询返回的行数和列数
    MYSQL *conn;//一个数据库链接
    int i,j;

    conn = mysql_init(NULL);

    if(conn == NULL) { 
        printf("mysql_init failed!\n");
        return;// EXIT_FAILURE;
    }

    conn = mysql_real_connect(conn,"localhost","lyt","","chat",0,NULL,0);
    if (conn) {
        //printf("Connection success!\n");
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
        res_ptr = mysql_store_result(conn);
        if(res_ptr) {
            column = mysql_num_fields(res_ptr);
            row = mysql_num_rows(res_ptr);
            if(row == 0) {
                //没有搜索到数据
                mes.mode=mode;
                mes.resault = -1;
                if(send(conn_fd,&mes,sizeof(MES),0) < 0) {
                    my_err("send",__LINE__);
                    exit(0);
                }
                mysql_close(conn);
                return;
            }
            //按行输出结果
            for(i = 1;i < row+1;i++){
                result_row = mysql_fetch_row(res_ptr);
                for(j = 0;j< column;j++) {
                    memset(&mes,0,sizeof(MES));
                    strcpy(mes.detail,result_row[j]);
                    mes.mode = mode;
                    if(send(conn_fd,&mes,sizeof(MES),0) < 0) {
                        my_err("send",__LINE__);
                        exit(0);
                    }
                }
            }
        }
    }
    //退出前关闭连接
    mysql_close(conn);
}

void del_fd(int conn_fd)
{
    U_L *temp,*temp1;
    temp1 = head;
    while(temp1!=NULL) {
        temp=temp1->next;
        if(temp->num == conn_fd) {
            break;
        }
        temp1=temp1->next;
    }
    temp1->next=temp->next;
    free(temp);
}

void logout(int conn_fd)
{
    MES mes;
    mes.mode = 10;
    if(send(conn_fd,&mes,sizeof(MES),0) < 0) {
        my_err("send",__LINE__);
        exit(0);
    }
    del_fd(conn_fd);
    close(conn_fd);
    pthread_exit(0);
}

void group_message(MES mes)
{
    int conn_fd;
    conn_fd=search_fd(mes.from);
    char sql[255];
    sprintf(sql,"SELECT * FROM history_group_%s;",mes.to);
    //puts(sql);
    int res;//执行sql语句后的返回标志
    MYSQL_RES *res_ptr;//指向查询结果的指针
    MYSQL_FIELD *field;//字段结构指针
    MYSQL_ROW result_row;//按行返回查询信息
    int row,column;//查询返回的行数和列数
    MYSQL *conn;//一个数据库链接
    int i,j;

    conn = mysql_init(NULL);

    if(conn == NULL) { 
        printf("mysql_init failed!\n");
        return;// EXIT_FAILURE;
    }

    conn = mysql_real_connect(conn,"localhost","lyt","","chat",0,NULL,0);
    if (conn) {
        //printf("Connection success!\n");
    } else {
        printf("Connection failed!\n");
    }
    mysql_query(conn,"set names gbk");//防止乱码。设置和数据库的编码一致就不会乱码

    MES mm;
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
            //按行输出结果
            for(i = 1;i < row+1;i++){
                result_row = mysql_fetch_row(res_ptr);
                memset(&mm,0,sizeof(MES));
                mm.mode = 12;
                strcpy(mm.detail,result_row[0]);
                strcat(mm.detail," ");
                strcpy(mm.from,result_row[1]);
                strcat(mm.detail,result_row[2]);
                if(send(conn_fd,&mm,sizeof(MES),0) < 0) {
                    my_err("send",__LINE__);
                    exit(0);
                }
            }
        }
    }
    //退出前关闭连接
    mysql_close(conn);

}

void one_message(MES mes)
{
    int conn_fd;
    conn_fd=search_fd(mes.from);
    char sql[255];
    if(strcmp(mes.from,mes.to) < 0) {
        sprintf(sql,"SELECT * FROM message_%s_%s;",mes.from,mes.to);
    } else {
        sprintf(sql,"SELECT * FROM message_%s_%s;",mes.to,mes.from);
    }
    //puts(sql);
    int res;//执行sql语句后的返回标志
    MYSQL_RES *res_ptr;//指向查询结果的指针
    MYSQL_FIELD *field;//字段结构指针
    MYSQL_ROW result_row;//按行返回查询信息
    int row,column;//查询返回的行数和列数
    MYSQL *conn;//一个数据库链接
    int i,j;

    conn = mysql_init(NULL);

    if(conn == NULL) { 
        printf("mysql_init failed!\n");
        return;// EXIT_FAILURE;
    }

    conn = mysql_real_connect(conn,"localhost","lyt","","chat",0,NULL,0);
    if (conn) {
        //printf("Connection success!\n");
    } else {
        printf("Connection failed!\n");
    }
    mysql_query(conn,"set names gbk");//防止乱码。设置和数据库的编码一致就不会乱码

    MES mm;
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
            //按行输出结果
            for(i = 1;i < row+1;i++){
                result_row = mysql_fetch_row(res_ptr);
                memset(&mm,0,sizeof(MES));
                mm.mode = 11;
                strcpy(mm.detail,result_row[0]);
                strcat(mm.detail," ");
                strcpy(mm.from,result_row[1]);
                strcat(mm.detail,result_row[3]);
                if(send(conn_fd,&mm,sizeof(MES),0) < 0) {
                    my_err("send",__LINE__);
                    exit(0);
                }
            }
        }
    }
    //退出前关闭连接
    mysql_close(conn);

}

void del_frined(MES mes)
{
    //在线发送通知
    if(search_fd(mes.to)) {
        if(send(search_fd(mes.to),&mes,sizeof(MES),0) < 0) {
            my_err("send",__LINE__);
            exit(0);
        }
    }

    //删除两人间的聊天记录

    char sql[255];
    if(strcmp(mes.from,mes.to) < 0) {
        sprintf(sql,"DROP TABLE message_%s_%s;",mes.from,mes.to);
    } else{
        sprintf(sql,"DROP TABLE message_%s_%s;",mes.to,mes.from); 
    }
    //puts(sql);
    MYSQL *conn;
    conn = mysql_init(NULL);
    if(conn == NULL) { 
        //如果返回NULL说明初始化失败
        printf("mysql_init failed!\n");
        return;// EXIT_FAILURE;
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
    //将各自从对方的好友列表里删除
    sprintf(sql,"DELETE from friend_%s WHERE name='%s' and mode=0;",mes.from,mes.to);
    //puts(sql);
    res = mysql_query(conn,sql);//正确返回0
    if(res) {
        perror("my_query");
        mysql_close(conn);
        exit(0);
    }
    sprintf(sql,"DELETE from friend_%s WHERE name='%s' and mode=0;",mes.to,mes.from);
    //puts(sql);
    mysql_query(conn,sql);

    mysql_close(conn);
}

void cha_pass(MES mes)
{
    char sql[255];
    sprintf(sql,"UPDATE info SET pass='%s' WHERE name='%s';",mes.detail,mes.from);
    //puts(sql);
    MYSQL *conn;
    conn = mysql_init(NULL);
    if(conn == NULL) { 
        //如果返回NULL说明初始化失败
        printf("mysql_init failed!\n");
        return;// EXIT_FAILURE;
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

void del_group_user(MES mes)
{
    char sql[255];
    sprintf(sql,"DELETE FROM group_%s WHERE name='%s';",mes.detail,mes.to);
    //puts(sql);
    MYSQL *conn;
    conn = mysql_init(NULL);
    if(conn == NULL) { 
        //如果返回NULL说明初始化失败
        printf("mysql_init failed!\n");
        return;// EXIT_FAILURE;
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

    sprintf(sql,"DELETE FROM friend_%s WHERE name='%s';",mes.to,mes.detail);
    //puts(sql);
    res = mysql_query(conn,sql);//正确返回0
    if(res) {
        perror("my_query");
        mysql_close(conn);
        exit(0);
    }

    mysql_close(conn);
}

void exit_group(MES mes)
{
    char sql[255];
    int res;//执行sql语句后的返回标志
    MYSQL_RES *res_ptr;//指向查询结果的指针
    MYSQL_FIELD *field;//字段结构指针
    MYSQL_ROW result_row;//按行返回查询信息
    int row,column;//查询返回的行数和列数
    MYSQL *conn;//一个数据库链接
    //初始化连接句柄
    conn = mysql_init(NULL);

    if(conn == NULL) { 
        //如果返回NULL说明初始化失败
        printf("mysql_init failed!\n");
        return;// EXIT_FAILURE;
    }

    conn = mysql_real_connect(conn,"localhost","lyt","","chat",0,NULL,0);
    if (!conn) {
        printf("Connection failed!\n");
    }
    mysql_query(conn,"set names gbk");//防止乱码。设置和数据库的编码一致就不会乱码

    sprintf(sql,"SELECT name FROM group_%s WHERE mode=1 and name='%s';",mes.to,mes.from);
    //puts(sql);
    //查询是否是群主
    res = mysql_query(conn,sql);//正确返回0
    if(res) {
        perror("my_query");
        mysql_close(conn);
        exit(0);
    } else{
        //把查询结果给res_ptr
        res_ptr = mysql_store_result(conn);
        if(res_ptr) {
           int ro;
            ro = mysql_num_rows(res_ptr);
            if(ro == 0){
                //如果结果为空,不是群主,退群处理
                sprintf(sql,"DELETE FROM group_%s WHERE name='%s';",mes.to,mes.from);
                puts(sql);
                mysql_query(conn,sql);
                sprintf(sql,"DELETE FROM friend_%s WHERE name='%s';",mes.from,mes.to);
                //puts(sql);
                mysql_query(conn,sql);
            } else {
                sprintf(sql,"SELECT name FROM group_%s;",mes.to);
                //puts(sql);
                mysql_query(conn,sql);
                res_ptr = mysql_store_result(conn);
                ro = mysql_num_rows(res_ptr);

                //是群主,解散群群处理
                char qun_user[ro][20];
				//printf("ro = %d\n",ro);
                int i;
                for(i=1;i<ro+1;i++) {
                    result_row = mysql_fetch_row(res_ptr);
					//将群从每个人的好友表中删除
					sprintf(sql,"DELETE FROM friend_%s WHERE name='%s';",result_row[0],mes.to);
					//puts(sql);
					mysql_query(conn,sql);
				}
				//删除该群的好友表
				sprintf(sql,"DROP TABLE group_%s;",mes.to);
				//puts(sql);
				mysql_query(conn,sql);
			
				//删除该群的消息记录表
				sprintf(sql,"DROP TABLE history_group_%s;",mes.to);
				//puts(sql);
				mysql_query(conn,sql);
			}
        }
    }
    mysql_close(conn);
}

void Sign_Ok(int conn_fd)
{
    //登陆成功后开始不断接收消息
    MES mes;
    while(1){
        int re;
        memset(&mes,0,sizeof(MES));
        re = recv(conn_fd,&mes,sizeof(MES),0);
        if(re < 0) {
            my_err("recv",__LINE__);
            exit(0);            
        } else if(re == 0) {
            printf("%s异常离线了\n",search_name(conn_fd));
            logout(conn_fd);
            pthread_exit(0);
        }
        //printf("re = %d\n",re);
        switch(mes.mode){
        case 3:
            //私聊
            chat_one(mes);
            break;
        case 4:
            //群聊
            printf("%s向群%s发送了一条消息\n",mes.from,mes.to);
            chat_group(mes);
            break;
        case 5:
            //所有人
            printf("%s 请求查看所有在线用户\n",mes.from);
            list_all(conn_fd);
            break;
        case 6:
            //好友
            printf("%s 请求查看好友列表\n",mes.from);
            list_friend(conn_fd,6,NULL);
            break;
        case 7:
            //群
            printf("%s 请求查看群列表\n",mes.from);
            list_friend(conn_fd,7,NULL);
            break;
        case 8:
            //添加好友
            printf("%s 请求添加好友\n",mes.from);
            add_friend(mes);
            break;
        case 9:
            //加群
            printf("%s邀请%s加入群%s\n",mes.from,mes.to,mes.detail);
            add_group(mes);
            break;
        case 10:
            printf("%s下线\n",mes.from);
            logout(conn_fd);
            break;
        case 11:
            printf("%s请求查看和%s的聊天记录\n",mes.from,mes.to);
            one_message(mes);
            break;
        case 12:
            printf("%s请求查看群%s的聊天记录\n",mes.from,mes.to);
            group_message(mes);
            break;
        case 13:
            printf("%s主动删除了和%s的好友关系\n",mes.from,mes.to);
            del_frined(mes);
            break;
        case 14:
            printf("%s将密码修改成\"%s\"了\n",mes.from,mes.detail);
            cha_pass(mes);
            break;
        case 15:
            printf("%s创建了群%s\n",mes.from,mes.to);
            creat_group(mes);
            break;
        case 16:
            printf("%s请%s离开群%s\n",mes.from,mes.to,mes.detail);
            del_group_user(mes);
            break;
        case 17:
            printf("%s解散/退出了群%s\n",mes.from,mes.to);
			exit_group(mes);
            break;
        case 18:
            printf("%s申请查看群%s的成员\n",mes.from,mes.to);
            list_friend(conn_fd,18,mes.to);
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
        re = recv(conn_fd,&mes,sizeof(MES),0);
        if(re < 0) {
            my_err("recv",__LINE__);
            exit(0);
        } else if(re == 0) {
            close(conn_fd);
            pthread_exit(0);
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
            //puts("res");
            //跳转到登陆或聊天页面
            add_list(name,conn_fd);
            printf("%s已登陆\n",name);

            Sign_Ok(conn_fd);
        }
    }
}

void send_sys_mes(void) 
{
    MES mes;
    while(1) {
        memset(&mes,0,sizeof(MES));
        scanf("%s",mes.detail);
        mes.mode = 19;
        U_L *temp = head->next;
        while(temp!=NULL) {
            send(temp->num,&mes,sizeof(MES),0);
            temp=temp->next;
        }
        puts("已发送全服广播!");
    }
}

int main()
{
    //发送全服消息线程
    pthread_t pp;
    pthread_create(&pp,NULL,(void*)send_sys_mes,NULL);
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
