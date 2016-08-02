/*************************************************************************
	> File Name: jointhread.c
	> Author: 
	> Mail: 
	> Created Time: 2016年08月01日 星期一 11时29分17秒
 ************************************************************************/

#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

void assisthread(void *arg)
{
    printf("I am helping to do some jobs\n");
    sleep(3);
    pthread_exit(0);
}

int main(void)
{
    pthread_t assistthid;
    int status;

    pthread_create(&assistthid,NULL,(void*)assisthread,NULL);
    pthread_join(assistthid,(void*)&status);
    printf("assisthreaf's exit is caused %d\n",status);

    return 0;
}
