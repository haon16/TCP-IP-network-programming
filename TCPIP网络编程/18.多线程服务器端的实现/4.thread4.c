//2020年9月28日 15:08:09
//多线程测试2   --与测试1相似，只是增加了发生临界区相关错误的可能性，即使在高配置系统环境下也容易验证产生的错误

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREAD 100
void* thread_inc(void *arg);
void* thread_des(void *arg);
long long num = 0;     //long long类型是64位整数型

int main(int argc, char *argv[])
{
    pthread_t thread_id[NUM_THREAD];

    printf("sizeof long long: %d \n", sizeof(long long));
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
    
    return 0;
}

void* thread_inc(void *arg)
{
    for (int i = 0; i < 50000000; i++)
        num += 1;       //临界区
    return NULL;
}

void* thread_des(void *arg)
{
    for (int i = 0; i < 50000000; i++)
        num -= 1;       //临界区
    return NULL;
}

/*
预期结果为0，但实际每次运行都是不同的结果
原因：多个线程访问同一个变量


值的计算需要CPU运算完成，需要先读到变量的值给CPU进行计算，然后再写入变量。
假设某一时刻num = 99，某一线程读到num值99，在+1计算以及重新写入之前，执行流程跳转到了另一线程取了num的值，此时还是99，
之后进行了+1计算以及写入结果100，此时当先前的线程切换得到CPU资源，会把计算结果100再一次写入变量，虽然两个线程各做了1次加1运算，却得到了意想不到的结果
因此线程访问变量num时应该阻止其他线程访问，直到线程1完成运算。这就是同步。


上面线程中的2个main函数中访问num的语句是临界区，这两条语句可能由多个线程同时运行，也是引起问题的直接原因
1.2个线程同时执行thread_inc函数
2.2个线程同时执行thread_des函数
3.2个线程分别执行thread_inc函数和thread_des函数 （两条不同语句由不同线程同时执行时，也有可能构成临界区，前提是这两条语句访问同一内存空间）
*/