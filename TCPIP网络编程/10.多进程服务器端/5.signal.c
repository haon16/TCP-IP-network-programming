//2020年9月4日 11:43:11
//信号：在特定事件发生时由操作系统向进程发送的消息
//信号处理：为了响应该消息，执行与消息相关的自定义操作

/*
"注册信号"过程：进程发现自己的子进程结束时，请求操作系统调用特定函数。
信号注册函数：   void (*signal(int signo, void (*fun)(int)))(int)    ->  为了在产生信号时调用，返回之前注册的函数指针
函数名：signal    
参数：  int signo            特殊情况信息
        void(*func)(int)    特殊情况下将要调用的函数的地址值（指针） 
返回类型：参数为int型，返回void型函数指针

部分特殊情况和对应的常数
SIGALRM :已到通过调用alarm函数注册的时间
SIGINT  :输入CTRL+C
SIGCHLD :子进程终止
*/

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig)       //分别定义信号处理函数，这种类型的函数称为信号处理器（Handler）
{
    if (sig == SIGALRM)
        puts("Time out!");
    alarm(2);               //每隔2s重复产生SIGALRM信号，在信号处理器中调用alarm函数
}

void keycontrol(int sig)
{
    if (sig == SIGINT)
        puts("CTRL+C pressed");
}

int main(int argc, char *argv[])
{
    signal(SIGALRM, timeout);    //注册SIGALRM,SIGINT信号及相应处理器
    signal(SIGINT, keycontrol);

    alarm(2);                   //传递一个正整形参数，相应时间后（以秒为单位）将产生SIGALRM信号。若向该函数传递0，则之前对SIGALRM信号的预约将取消。如果通过该函数
                                //预约信号后未指定该信号对应的处理函数，则（通过调用signal函数）终止进程，不做任何处理
                                //返回0或以秒为单位的距SIGALRM信号发生所剩时间

    for (int i = 0; i < 3; i++)
    {
        puts("wait...");         //实际测试没有CTRL+C终止时只输出了2次wait信息，CTRL+C终止时一共输出了3次wait信息，这个没搞明白
        sleep(100);              //发生信号时将唤醒由于调用sleep函数而进入阻塞状态的进程，一旦被唤醒就不会再进入睡眠状态
    }

    return 0;
}