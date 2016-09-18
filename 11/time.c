/*************************************************************************
	> File Name: time.c
	> Author: 
	> Mail: 
	> Created Time: 2016年08月15日 星期一 15时13分00秒
 ************************************************************************/

#include<stdio.h>
#include<time.h>
main()
{
    time_t timep;
    struct tm *p;
    time(&timep);
    p=localtime(&timep);
    //获取当地时间
    char time[30];
    sprintf(time,"%d.%d.%d %d:%d:%d",(1900+p->tm_year),(1+p->tm_mon), p->tm_mday,p->tm_hour, p->tm_min, p->tm_sec);
    puts(time);
}
