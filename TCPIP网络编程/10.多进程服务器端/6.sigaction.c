//2020年9月8日 15:26:58
//利用sigaction函数进行信号处理

/*
sigaction函数类似于signal函数，而且完全可以代替后者，也更稳定。
因为signal函数在UNIX系列的不同操作系统中可能存在区别，但sigaction函数完全相同
实际上现在很少使用signal函数编写程序，它只是为了保持对旧程序的兼容

int sigaction(int signo, const struct sigaction *act, struct sigaction *oldact);
signo  与signal函数相同，传递信号信息
act    对应于第一个参数的信号处理函数（信号处理器）信息
oldact 通过此参数获取之前注册的信号处理函数指针，若不需要则传递0

struct sigaction
{
    void (*sa_handler)(int);          sa_handler保存信号处理函数的地址值
    sigset_t sa_mask;                 sa_mask和sa_flags用于指定信号相关的选项和特性，这边只是防止产生僵尸进程，所有位初始化为0就行
    int sa_flags;
}
*/

#define _XOPEN_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig)
{
    if (sig == SIGALRM)
        puts("Time out");
    alarm(2);
}

int main(int argc, char *argv[])
{
    struct sigaction act;
    act.sa_handler = timeout;
    sigemptyset(&act.sa_mask);    //调用sigemptyset函数将sa_mask成员的所有位初始化为0
    act.sa_flags = 0;

    sigaction(SIGALRM, &act, 0);  //注册SIGALRM信号的处理器
    alarm(2);

    for (int i = 0; i < 3; i++)
    {
        puts("wait...");
        sleep(100);
    }

    return 0;
}