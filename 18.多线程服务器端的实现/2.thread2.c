//2020年9月26日 16:13:54
//控制线程的执行流

/*
int pthread_join(pthread_t thread, void ** status)
    thread:该参数值ID的线程终止后才会从该函数返回
    status:保存线程的main函数返回值的指针变量地址值
    成功时返回0，失败时返回其他值

作用：调用该函数的进程（或线程）将进入等待状态，直到第一个参数为ID的线程终止为止。而且可以得到线程的main函数返回值
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void* thread_main(void *arg);

int main(int argc, char *argv[])
{
    pthread_t t_id;
    int thread_param = 5;
    void *thr_ret;

    if (pthread_create(&t_id, NULL, thread_main, (void *)&thread_param) != 0)
    {
        puts("pthread_create() error");
        return -1;
    }

    if (pthread_join(t_id, &thr_ret) != 0)      //main函数将等待ID保存在t_id变量中的线程终止
    {
        puts("pthread_join() error");
        return -1;
    }

    printf("Thread return message:%s \n", (char *)thr_ret);
    free(thr_ret);
    return 0;
}

void* thread_main(void *arg)
{
    int cnt = *((int *)arg);
    char *msg = (char *)malloc(sizeof(char)*50);
    strcpy(msg, "Hello, I'am thread~ \n");

    for (int i = 0; i < cnt; i++)
    {
        sleep(1);
        puts("running thread");
    }
    return (void *)msg;         //返回值是thread_main函数内部动态分配的内存空间地址值
}

