//2020年9月28日 17:12:29
//互斥量

/*
线程同步用于解决线程访问顺序引发的问题。需要同步的情况可以从如下两方面考虑
1.同时访问用一内存空间时发生的情况              
2.需要指定访问同一内存空间的线程执行顺序的情况

1.如thread4
2.假设有A，B两个线程，线程A负责向指定内存空间写入（保存）数据，线程B负责取走该数据。这种情况下，
  线程A首先应该访问约定的内存空间并保存数据。万一线程B先访问并取走数据，将导致错误结果。像这种需要控制线
  程执行顺序的情况也需要使用同步技术
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREAD 100
void* thread_inc(void *arg);
void* thread_des(void *arg);

long long num = 0;
pthread_mutex_t mutex;          //声明了保存互斥量读取值的变量，互斥量声明为全局变量因为thread_inc和thread_des都需要访问

int main(int argc, char *argv[])
{
    pthread_t thread_id[NUM_THREAD];
    pthread_mutex_init(&mutex, NULL);   //互斥量创建，参数2是传递即将创建的互斥量属性，没有特别需要指定的属性时传递NULL

    for (int i = 0; i < NUM_THREAD; i++)
    {
        if (i % 2)
            pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
        else
            pthread_create(&(thread_id[i]), NULL, thread_des, NULL);
    }

    for (int i = 0; i < NUM_THREAD; i++)
        pthread_join(thread_id[i], NULL);

    printf("result: %lld \n", num);
    pthread_mutex_destroy(&mutex);      //销毁互斥量
    return 0;
}

void* thread_inc(void *arg)
{
    pthread_mutex_lock(&mutex);             //调用pthread_mutex_lock函数时，发现有其他线程已进入临界区，则pthread_mutex_lock函数不会返回，直到里面的线程
    for (int i = 0; i < 50000000; i++)      //调用pthread_mutex_unlock函数退出临界区为止。也就是说，其他线程让出临界区之前，当前线程将一直处于阻塞状态。
        num += 1;
    pthread_mutex_unlock(&mutex);           //利用lock和unlock函数围住临界区的两端。此时互斥量相当于一把锁，阻止多个线程同时访问。   
    return NULL;                            //退出线程临界区时，如果忘了调用pthread_mutex_unlock函数，其他为了进入临界区而调用pthread_mutex_lock函数的线程就无法摆脱阻塞状态。这种情况称为“死锁”
}

void* thread_des(void *arg)
{             
    for (int i = 0; i < 50000000; i++)      //thread_des函数比thread_inc函数多调用49999999次互斥量lock、unlock函数，运行消耗时间十分长
    {
        pthread_mutex_lock(&mutex);
        num -= 1;
        pthread_mutex_unlock(&mutex);    
    }       
    return NULL;                           
}


//thread_inc中的临界区划分范围比thread_des大，这是为了最大限度减少互斥量lock、unlock函数的调用次数
//如果不太关注线程的等待时间，可以适当扩展临界区。但变量num的值增加到50000000前不允许其他线程访问，反而成立缺点。
//这里没有正确答案，需要根据不同程序酌情考虑究竟扩大还是缩小临界区。