/*************************************************************************
	> File Name: ioctl.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月19日 星期二 08时40分14秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<net/if.h>
#include<string.h>

char g_eth_name[16];//以太网名
char g_macaddr[6];//mac地址
unsigned int g_subnetmask;//子网掩码
unsigned int g_ipaddr;//ip地址
unsigned int g_broadcast_ipaddr;//广播ip地址

//初始化网络，获取当前网络设备信息
void init_net(void)
{
    int i;
    int sock;
    struct sockaddr_in sin;
    struct ifreq ifr;

    sock = socket(AF_INET,SOCK_DGRAM,0);
    if(sock == -1) {
        perror("socket");
    }

    strcpy(g_eth_name,"eth0");
    strcpy(ifr.ifr_name,g_eth_name);
    printf("ethname:\t%s\n",g_eth_name);

    //获取并打印网卡地址
    if (ioctl(sock,SIOCGIFHWADDR,&ifr) < 0) {
        perror("ioctl");
    }
    memcpy(g_macaddr,ifr.ifr_hwaddr.sa_data,6);//复制内存
    printf("local mac:\t");
    for(i=0;i<5;i++) {
        printf("%.2x:",g_macaddr[i]);
    }
    printf("%.2x\n",g_macaddr[i]);

    //获取并打印IP地址
    if(ioctl(sock,SIOCGIFADDR,&ifr) < 0) {
        perror("ioctl");
    }

    memcpy(&sin,&ifr.ifr_addr,sizeof(sin));
    g_ipaddr = sin.sin_addr.s_addr;
    printf("local eth0;\t%s\n",inet_ntoa(sin.sin_addr));

    //获取并打印广播地址
    if(ioctl(sock,SIOCGIFBRDADDR,&ifr) < 0) {
        perror("ioctl");
    }
    memcpy(&sin,&ifr.ifr_addr,sizeof(sin));
    g_broadcast_ipaddr = sin.sin_addr.s_addr;
    printf("broadcast:\t%s\n",inet_ntoa(sin.sin_addr));

    //获取并打印子网掩码
    if(ioctl(sock,SIOCGIFNETMASK,&ifr) < 0) {
        perror("ioctl");
    }
    memcpy(&sin,&ifr.ifr_addr,sizeof(sin));
    g_subnetmask = sin.sin_addr.s_addr;
    printf("subnetmask:\t%s\n",inet_ntoa(sin.sin_addr));

    close(sock);
}

int main()
{
    /*initialize...*/
    init_net();

    /*do something*/
    return 0;
}
