//2020年9月12日 10:53:11
//通过管道实现进程间通信

/*
进程之间具有完全独立的内存结构，就连通过fork函数创建的子进程也不会与父进程共享内存空间，因此进程间通信只能通过其他特殊方法完成
管道并非属于进程的资源，而是和套接字一样，属于操作系统（也就不是fork函数的复制对象）。所以两个进程通过操作系统提供的内存空间进行通信。
    int pipe(int filedes[2])
filedes[0] : 通过管道接收数据时使用的文件描述符，即管道出口
filedes[1] : 通过管道传输数据时使用的文件描述符，即管道入口
返回值：成功时返回0，失败时返回-1
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    int fds[2];
    char str[] = "Who are you?";
    char buf[BUF_SIZE];

    pipe(fds);              //调用pipe函数创建管道，同时获取对应于出入口的文件描述符
    pid_t pid = fork();     //通过fork函数将文件描述符传递给子进程
    if (pid == 0)
    {
        write(fds[1], str, sizeof(str));
    }
    else
    {
        read(fds[0], buf, BUF_SIZE);
        puts(buf);
    }

    return 0;
}

//父子进程都可以访问管道的I/O路径，但这边子进程仅用输入路径，父进程仅用输出路径