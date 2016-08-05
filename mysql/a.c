/*************************************************************************
	> File Name: a.c
	> Author: 
	> Mail: 
	> Created Time: 2016年08月04日 星期四 16时50分47秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<mysql/mysql.h>

int main(void)
{
    MYSQL *conn;
    //初始化连接句柄

    conn = mysql_init(NULL);

    if(conn == NULL) { //如果返回NULL说明初始化失败
        printf("mysql_init failed!\n");
        return EXIT_FAILURE;
    }

    //进行实际连接
    //参数　conn连接句柄，host　mysql所在的主机或地址,user用户名,passwd密码,database_name数据库名,后面的都是默认
    conn = mysql_real_connect(conn,"localhost","lyt","","chat_room",0,NULL,0);
    if (conn) {
        printf("Connection success!\n");
    } else {
        printf("Connection failed!\n");
    }
    mysql_query(conn,"set names gbk");//防止乱码。设置和数据库的编码一致就不会乱码


    //退出前关闭连接
    mysql_close(conn);

    return 0;
}
