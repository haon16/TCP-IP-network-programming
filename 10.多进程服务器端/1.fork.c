//2020年9月2日 17:36:43
//通过调用fork函数创建进程

/*
并发服务器端的实现方法：
多进程服务器：通过创建多个进程提供服务       （Windows不支持）
多路复用服务器：通过捆绑并统一管理I/O对象提供服务
多线程服务器：通过生成与客户端等量的线程提供服务

进程：占用内存空间的正在运行的程序
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int gval = 10;
int main(int argc, char *argv[])
{
    pid_t pid;
    int lval = 20;
    gval++;
    lval += 5;

    pid = fork();       //fork函数将创建调用的进程副本。也就是说并非根据完全不同的程序创建进程，而是复制正在运行的、调用fork函数的进程。
                        //在fork函数返回后，两个进程都将执行fork函数调用后的语句，但因为通过同一个进程、复制相同的内存空间，之后的程序流要根据fork函数的返回值加以区分
    if (pid == 0)       //子进程:fork函数返回0
    {
        gval += 2;
        lval += 2;
    }
    else                //父进程：fork函数返回子进程ID
    {
        gval -= 2;
        lval -= 2;
    }
    
    if (pid == 0)
        printf("Child Proc: [%d, %d] \n", gval, lval);
    else
        printf("Parent Proc: [%d %d], Child pid: %d \n", gval, lval, pid);

    return 0;
}
