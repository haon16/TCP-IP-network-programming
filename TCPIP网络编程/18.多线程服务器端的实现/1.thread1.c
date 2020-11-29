//2020年9月26日 14:33:34
//线程的创建

/*
多进程模型的缺点：1.创建进程的过程中会带来一定的开销
                2.为了完成进程间数据交换，需要特殊的IPC技术
                3.每秒少则数十次，多则数千次的‘上下文切换’是创建进程时最大的开销！！！

线程相比于进程具有的优点：1.线程的创建和上下文切换比进程的创建和上下文切换更快
（轻量级进程）           2.线程间交换数据时无需特殊技术

线程是为了解决如下困惑登场的：为了得到多条代码执行流而复制整个内存区域的负担太重了 （包括数据区，堆区域，栈区域）
但如果以获得多个代码执行流为主要目的，则不应该像多进程那样完全分离内存结构，而只需分离栈区域。
通过这种方式可以获得如下优势： 1.上下文切换时不需要切换数据区和堆
                            2.可以利用数据区和堆交换数据
实际上这就是线程。线程为了保持多条代码执行流而隔离了栈区域，同一进程内的多个线程将共享数据区和堆


int pthread_create (pthread_t * restrict thread, const pthread_attr_t * restrict attr, void *(*start_routine) (void *), void * restrict arg)
    thread ：保存新创建线程ID的变量地址值  
    attr   ：用于传递线程属性的参数，传递NULL时，创建默认属性的线程
    start_routine：相当于线程main函数的、在单独执行流中执行的函数地址值（函数指针）
    arg ：通过第三个参数传递调用函数时包含传递参数信息的变量地址值
    成功时返回0，失败时返回其他值

线程相关代码在编译时需要添加-lpthread选项声明需要连接线程库，只有这样才能调用头文件pthread.h中声明的函数
*/

#include <stdio.h>
#include <pthread.h>

void* thread_main(void *arg);

int main(int argc, char *argv[])
{
    pthread_t t_id;
    int thread_param = 5;
     
    if (pthread_create(&t_id, NULL, thread_main, (void*)&thread_param) != 0)
    {
        puts("thread_create() error");
        return -1;
    }
    sleep(10);              //延迟进程的终止时间       
    puts("end of main");
    return 0;               //执行return语句后会终止进程，同时终止内部创建的线程
}

void* thread_main(void *arg)
{
    int cnt = *((int *)arg);
    for (int i = 0; i < cnt; i++)
    {
        sleep(1);
        puts("running thread");
    }
    return NULL;
}


//43行的sleep函数改成2s的话，运行后不会输出5次"running thread"，因为main函数返回后整个进程将被销毁