/*************************************************************************
	> File Name: test_address.c
	> Author: 
	> Mail: 
	> Created Time: 2016年08月02日 星期二 15时03分42秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main()
{
    char buffer[32];
    int ret = 0;
    int host = 0;
    int network = 0;
    unsigned int address = 0;
    struct in_addr in;

    in.s_addr = 0;

    //输入一段ip
    printf("please input your ip address:");
    fgets(buffer,31,stdin);
    buffer[31]= '\0';

    //示例inet_aton函数
    if((ret = inet_aton(buffer,&in)) == 0) {
        printf("inet_aton: \t invalid address\n");
    } else {
        printf("inet_aton:\t0x%x\n",in.s_addr);
    }

    //示例inet_addr函数
    if((address = inet_addr(buffer)) == INADDR_NONE) {
        printf("inet_addr: \t invalid address\n");
    } else {
        printf("inet_addr:\t0x%1x\n",address);
    }

    //示例inet_network函数
    if((address = inet_network(buffer)) == -1) {
        printf("inet_network: \t invalid address\n");
    } else {
        printf("inet_network:\t0x%1x\n",address);
    }

    //示例使用inet_ntoa函数
    if(inet_ntoa(in) == NULL) {
        printf("inet_ntoa: \t invalid address\n");
    } else {
        printf("inet_ntoa:\t%s\n",inet_ntoa(in));
    }

    //示例inet_lnaof与inet_netof函数
    host = inet_lnaof(in);
    network = inet_netof(in);
    printf("inet_lnaof:\t0x%x\n",host);
    printf("inet_netof:\t0x%x\n",network);

    in = inet_makeaddr(network,host);
    printf("inet_makeaddr:0x%x\n",in.s_addr);

    return 0;
}
