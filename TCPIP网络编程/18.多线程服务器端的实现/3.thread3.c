//2020年9月28日 14:49:33
//多线程测试1

#include <stdio.h>
#include <pthread.h>

void* thread_summation(void *arg);
int sum = 0;

int main(int argc, char *argv[])
{
    pthread_t id_t1;
    pthread_t id_t2;
    int range1[] = {1, 5};
    int range2[] = {6, 10};

    pthread_create(&id_t1, NULL, thread_summation, (void *)range1);
    pthread_create(&id_t2, NULL, thread_summation, (void *)range2);

    pthread_join(id_t1, NULL);
    pthread_join(id_t2, NULL);
    printf("result: %d \n", sum);

    return 0;
}

void* thread_summation(void *arg)
{
    int start = ((int *)arg)[0];
    int end = ((int *)arg)[1];

    while (start <= end)
    {
        sum += start;           //2个线程都可直接访问全局变量sum，原因在于2个线程共享保存全局变量的数据区
        start++;
    }
    return NULL;
}


//运行结果为55，虽然正确的，但示例本身存在问题，此处存在临界区相关问题