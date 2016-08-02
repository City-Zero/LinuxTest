/*************************************************************************
	> File Name: createthread.c
	> Author: 
	> Mail: 
	> Created Time: 2016年08月01日 星期一 09时11分50秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

int *thread(void *arg)
{
    pthread_t newthid;

    newthid = pthread_self();
    printf("this is a new thread,thread ID =%lu\n",newthid);
    return NULL;
}

int main(void)
{
    pthread_t thid;

    printf("main thread,ID id %lu\n",pthread_self());//Print main thread ID
    if(pthread_create(&thid,NULL,(void*)thread,NULL) != 0) {
        printf("thread creation failed\n");
        exit(1);
    }
    sleep(1);
    exit(0);
}
