/*************************************************************************
	> File Name: te.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月29日 星期五 15时06分25秒
 ************************************************************************/

#include<stdio.h>
#include<readline/readline.h>
#include<readline/history.h>

int main()
{
    char *line;
    while(1) {
        line = readline(">>");
        puts(line);
        if(*line) {
            add_history(line);
        }
        free(line);
    }
    return 0;
}
