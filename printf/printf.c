/*************************************************************************
	> File Name: printf.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月30日 星期六 11时06分40秒
 ************************************************************************/

#include<stdio.h>
#include<stdarg.h>
int my_printf(const char *fmt, ...)  
{  
//        char printf_buf[1024];  
    va_list args;  
    int printed;  

    va_start(args, fmt);
	int i;
	int j=1;
	char ch;
	while(ch=*fmt++) {
		if(ch='%') {
			ch=*fmt++;
			switch(ch) {
			case 'c':
				char ch1=va_arg(args,char);
				putchar(ch1);
				break;
			case 's':
				char *ch2=va_arg(args,char*);
				puts(ch2);
				break;
			}
		} else{
			putchar(ch);
		}
	}
//    printed = vsprintf(printf_buf, fmt, args);  
    va_end(args);  
  
//    puts(printf_buf);  
  
    return printed;  
}  
int main()
{
    char a,b[]="HEll",c,d,f;
    a='A';
    c='c';
    d='d';
    f='f';
    my_printf("^^%c %s",a,b);
}
