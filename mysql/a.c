/*************************************************************************
	> File Name: a.c
	> Author: 
	> Mail: 
	> Created Time: 2016年08月04日 星期四 16时50分47秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<mysql/mysql.h>

int main(void)
{
    char *sql;
    sql="SELECT name FROM info WHERE name='sss';";
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
            printf("查到%d行,%d列\n",row,column);
            //输出结果的字段名
            for(i = 0;field = mysql_fetch_field(res_ptr);i++) {
                printf("%10s",field->name);
            }
            puts("");
            //按行输出结果
            for(i = 1;i < row+1;i++){
                result_row = mysql_fetch_row(res_ptr);
                for(j = 0;j< column;j++) {
                    printf("%10s",result_row[j]);
                }
                puts("");
            }
        }
    }
    //退出前关闭连接
    mysql_close(conn);

    return 0;
}
