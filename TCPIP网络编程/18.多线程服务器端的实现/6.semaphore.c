//2020年9月29日 10:23:21
//信号量   --此处只涉及利用“二进制信号量”（只用0和1）完成“控制线程顺序”为中心的同步方法

/*
int sem_init(sem_t *sem, int pshared, unsigned int value)
sem : 创建信号量时传递保存信号量的变量地址值
pshared : 传递其他值时，创建可由多个进程共享的信号量；传递0时，创建只允许1个进程内部使用的信号量
value : 指定新创建的信号量初始值


调用sem_init函数时，操作系统将创建信号量对象，此对象中记录着“信号量值”整数。该值在调用sem_post函数时增1，调用sem_wait函数时减1。
但信号量的值不能小于0，因此，在信号量为0的情况下调用sem_wait函数时，调用函数的线程将进入阻塞状态（因为函数未返回）。当然，此时如果
有其他线程调用sem_post函数，信号量的值将变为1，而原本阻塞的线程可以将该信号量重新减为0并跳出阻塞状态。实际上就是通过这种特性完成
临界区的同步操作。
    sem_wait(&sem)    //信号量变为0
    //临界区的开始
    //. . . . .
    //临街区的结束
    sem_post(&sem);   //信号量变为1
上述代码结构中，调用sem_wait函数进入临界区的线程在调用sem_post函数前不允许其他线程进入临界区。
信号量的值在0和1之间跳转，因此，具有这种特性的机制称为“二进制信号量”
*/


#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void* read(void *arg);
void* accu(void *arg);
static sem_t sem_one;
static sem_t sem_two;
static int num;

int main(int argc, char *argv[])
{
    pthread_t id_t1, id_t2;
    sem_init(&sem_one, 0, 0);       //生成2个信号量，一个信号量的值为0，另一个为1
    sem_init(&sem_two, 0, 1);

    pthread_create(&id_t1, NULL, read, NULL);
    pthread_create(&id_t2, NULL, accu, NULL);

    pthread_join(id_t1, NULL);
    pthread_join(id_t2, NULL);

    sem_destroy(&sem_one);
    sem_destroy(&sem_two);

    return 0;
}

void* read(void *arg)
{
    for (int i = 0; i < 5; i++)
    {
        fputs("Input num: ", stdout);
        sem_wait(&sem_two);
        scanf("%d", &num);
        sem_post(&sem_one);
    }
    return NULL;
}

void* accu(void *arg)
{
    int sum = 0;
    for (int i = 0; i < 5; i++)
    {
        sem_wait(&sem_one);
        sum += num;
        sem_post(&sem_two);
    }
    printf("Result: %d \n", sum);
    return NULL;
}

//错位调用wait和post函数保证了先读后算
//利用信号量变量sem_two调用wait和post函数。这是为了防止在调用accu函数的线程还未取走数据的情况下，调用read函数的线程覆盖原值
//利用信号量变量sem_one调用wait和post函数。这是为了防止调用read函数的线程写入新值前，accu函数取走（再取走旧值）数据
