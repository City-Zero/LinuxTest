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

int match(int mode,char *ch)
{
    char *sql;
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
    strcpy(pass,name[i]);
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
        sql="SELECT name FROM info;";//查询注册是否重名
        res = mysql_query(conn,sql);//正确返回0
        if(res) {
            perror("my_query");
            mysql_close(conn);
            exit(0);
        } else{
            //把查询结果给res_ptr
            res_ptr = mysql_store_result(conn);
            //如果结果不为空,则输出
            if(res_ptr) {
                column = mysql_num_fields(res_ptr);
                row = mysql_num_rows(res_ptr);
                for(i = 1;i < row+1;i++){
                    result_row = mysql_fetch_row(res_ptr);
                    if(strcmp(result_row[0],name) == 0) {
                        pan = 1;
                        break;
                    }
                }
            }
        }
		if(pan) {
		char s[40];
		strcpy(s,"INSERT INTO info VALUES('");
		strcat(s,name)
		mysql_query(conn,"INSERT INTO info VALUES('n','n');");
		}
    } else if(mode == 1) {
        sql="SELECT * FROM info;";//查询登陆是否正确
    	res = mysql_query(conn,sql);//正确返回0
    	if(res) {
        	perror("my_query");
        	mysql_close(conn);
        	exit(0);
    	} else{
        	//把查询结果给res_ptr
        	res_ptr = mysql_store_result(conn);
        	//如果结果不为空,则输出
        	if(res_ptr) {
            	column = mysql_num_fields(res_ptr);
	            row = mysql_num_rows(res_ptr);
    	        //输出结果的字段名
    	        for(i = 0;field = mysql_fetch_field(res_ptr);i++) {
    	        }
    	        //按行输出结果
    	        for(i = 1;i < row+1;i++){
    	            result_row = mysql_fetch_row(res_ptr);
    	            if(strcmp(result_row[0],name) == 0 && strcmp(result_row[1],pass) == 0) {
						pan = 1;
						break;
					}
    	        }
    	    }
    	}
    }
    //退出前关闭连接
    mysql_close(conn);

    return 0;
}

void sign_in_up(int conn_fd)
{
    MES mes;
    memset(&mes,0,sizeof(MES));
    if(recv(conn_fd,&mes,sizeof(MES),0) < 0) {
        my_err("recv",__LINE__);
        exit(0);
    }
    int res;
    res = match(mes.mode,mes.detail);
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
        printf("一个新的连接,来自 %s\n",inet_ntoa(cli_addr.sin_addr));

        pthread_t thid;
        pthread_create(&thid,NULL,(void*)sign_in_up(),&conn_fd)
    }
}
