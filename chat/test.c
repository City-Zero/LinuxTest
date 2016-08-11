/*************************************************************************
	> File Name: test.c
	> Author: 
	> Mail: 
	> Created Time: 2016年08月09日 星期二 21时56分28秒
 ************************************************************************/

#include<stdio.h>
int main()
{
    char name[]="lyt";
    char pass[]="lyt";
    char ch[50];
    sprintf(ch,"INSERT INTO info(name,pass) VALUES('%s','%s');",name,pass);
    puts(ch);
}
