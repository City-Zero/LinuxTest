/*************************************************************************
	> File Name: b.c
	> Author: 
	> Mail: 
	> Created Time: 2016年08月09日 星期二 09时25分53秒
 ************************************************************************/

#include <stdio.h>
#include "/usr/include/mysql/mysql.h"
/*数据库连接用宏*/
#define HOST "localhost"
#define USERNAME "lyt"
#define PASSWORD ""
#define DATABASE "chat_room"

int main()
{
        char *query;
        query="select * from user_message";
        query_sql(query);
        return 0;

}
void query_sql(char* sql)
{
        MYSQL my_connection; /*这是一个数据库连接*/
        int res; /*执行sql語句后的返回标志*/
        MYSQL_RES *res_ptr; /*指向查询结果的指针*/
        MYSQL_FIELD *field; /*字段结构指针*/
        MYSQL_ROW result_row; /*按行返回的查询信息*/
            int row, column; /*查询返回的行数和列数*/
        int i, j;
        /*初始化mysql连接my_connection*/
            mysql_init(&my_connection);
        /*建立mysql连接*/
        if (NULL != mysql_real_connect(&my_connection, HOST, USERNAME, PASSWORD,
                                                                         DATABASE, 0, NULL, CLIENT_FOUND_ROWS))  /*连接成功*/
    {
                printf("数据库查询query_sql连接成功！\n");
                /*设置查询编码为gbk，以支持中文*/
                mysql_query(&my_connection, "set names gbk");
                res = mysql_query(&my_connection, sql);
                if (res)   /*执行失败*/
        {
                        printf("Error： mysql_query !\n");
                        /*关闭连接*/
                                    mysql_close(&my_connection);
                    
        }
                else     /*现在就代表执行成功了*/
        {
                        /*将查询的結果给res_ptr*/
                        res_ptr = mysql_store_result(&my_connection);
                        /*如果结果不为空，就把结果print*/
                                    if (res_ptr)
            {
                                /*取得結果的行数和*/
                                                column = mysql_num_fields(res_ptr);
                                row = mysql_num_rows(res_ptr);
                                printf("查询到 %d 行 \n", row);
                                /*输出結果的字段名*/
                                for (i = 0; field = mysql_fetch_field(res_ptr); i++)
                                    printf("%10s ", field->name);
                                printf("\n");
                                /*按行输出結果*/
                                for (i = 1; i < row+1; i++)
                {
                                        result_row = mysql_fetch_row(res_ptr);
                                        for (j = 0; j < column; j++)
                                            printf("%10s ", result_row[j]);
                                        printf("\n");
                                    
                }
                            
            }
                        /*不要忘了关闭连接*/
                                    mysql_close(&my_connection);
                    
        }
            
    }
    else
    {
                printf("数据库连接失败");
            
    }

}
