//2020年9月3日 15:50:25
//创建僵尸进程：完成工作后（执行完main函数中的程序后）没被销毁，继续占用系统中的重要资源的进程

/*
产生僵尸进程的原因
调用fork函数产生子进程的终止方式：1.传递参数并调用exit函数   2.main函数中执行return语句并返回值
向exit函数传递的参数值和main函数的return语句返回的值都会传递给操作系统。而操作系统不会销毁子进程，直到把这些值传递给产生该子进程的父进程。
处在这种状态下的进程就是僵尸进程。也就是说，让子进程编程僵尸进程的正是操作系统。

销毁僵尸进程：向创建子进程的父进程传递子进程的exit参数值或return语句的返回值
操作系统不会主动把这些值传递给父进程，只有父进程主动发起请求（函数调用）时，操作系统才会传递该值。
如果父进程未主动要求获得子进程的结束状态值，操作系统将一直保存，并让子进程长时间处于僵尸进程状态。
也就是说父母要负责收回自己生的孩子
*/


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    pid_t pid = fork();

    if (pid == 0)
    {
        puts("Hi, I am a child process");
    }
    else
    {
        printf("Child Process ID: %d \n", pid);
        sleep(30);
    }
    
    if (pid == 0)
        puts("End child process");
    else
        puts("End parent process");

    return 0;
}