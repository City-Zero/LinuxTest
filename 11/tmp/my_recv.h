/*************************************************************************
	> File Name: recv.h
	> Author: 
	> Mail: 
	> Created Time: 2016年08月04日 星期四 09时46分18秒
 ************************************************************************/

#ifndef _RECV_H
#define _RECV_H
    #define BUFSIZE 1024
    void my_err(const char * err_string,int line);
    int my_recv(int conn_fd,char *data_buf,int len);
#endif
