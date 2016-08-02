/*************************************************************************
	> File Name: condition.c
	> Author: 
	> Mail: 
	> Created Time: 2016年08月01日 星期一 16时24分02秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>

pthread_mutex_t mutex;
pthread_cond_t cond;

void *thread1(void *arg)
{
    pthread_cleanup_push(pthread_mutex_unlock,&mutex);

    while(1) {
        printf("thread1 is running\n");
        pthread_mutex_lock (&mutex);//add lock
        pthread_cond_wait (&cond,&mutex);//无条件等待
        printf("thread1 applied the condition\n");
        pthread_mutex_unlock (&mutex);
        sleep(4);
    }
    pthread_cleanup_pop(0);
}

void *thread2(void *arg)
{
    while(1) {
        printf("thread2 is running\n");
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond,&mutex);
        printf("thread2 applied the condition\n");
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}

int main(void)
{
    pthread_t tid1,tid2;
    printf("condition variable study!\n");
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);
    pthread_create(&tid1,NULL,(void*)thread1,NULL);
    pthread_create(&tid2,NULL,(void*)thread2,NULL);

    do{
        pthread_cond_signal(&cond);
    } while(1);

    sleep(20);
    puts("lkjhf");
//    pthread_exit(0);
//    return 0;
}
