//2020年9月14日 15:08:46
//select函数调用示例

/*
多进程服务器端的缺点：创建进程代价较大，需要大量的运算和内存空间，由于每个进程都具有独立的内存空间，所以相互间的数据交换也要求采用相对复杂的方法（IPC属于相对复杂的通信方法）
多进程服务器端的解决方案：基于I/O复用的服务器端（通过1个进程向多个客户端提供服务）


int select (int maxfd, fd_set *readset, fd_set *writeset, fd_set *exceptset,
            const struct timeval *timeout);
maxfd ：监视对象文件描述符数量
readset : 将所有关注“是否存在待读取数据”的文件描述符注册到fd_set型变量，并传递其地址值
writeset : ..."是否可传输无阻塞数据"...
exceptset : ..."是否发生异常"...
timeout ：调用select函数后，为防止陷入无限阻塞的状态，传递超时（time-out）信息
返回值：发生错误时返回-1，超时返回时返回0。因发生关注的事件返回时，返回大于0的值，该值是发生事件的文件描述符数
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    fd_set reads, temps;
    FD_ZERO(&reads);
    FD_SET(0, &reads);          //文件描述符为0的是标准输入，监视标准输入的变化

    struct timeval timeout;
    /*
    timeout.tv_sec = 5;         //不能再此时设置超时，调用select函数后，结构体timeval的成员值会被替换为超时前剩余时间
    timeout.tv_usec = 5000;
    */
    char buf[BUF_SIZE];
    int result, str_len;
    while (1)
    {
        temps = reads;          //初始值保持不变 
        timeout.tv_sec = 5;     //每次调用select函数前都需要初始化timeval结构体变量
        timeout.tv_usec = 0;
        result = select(1, &temps, 0, 0, &timeout);
        if (result == -1)
        {
            puts("select() error!");
            break;
        }
        else if (result == 0)
        {
            puts("Time-out!");
        }
        else
        {
            if (FD_ISSET(0, &temps))    //验证发生变化的文件描述符是否为标准输入
            {
                str_len = read(0, buf, BUF_SIZE);
                buf[str_len] = 0;
                printf("message from console: %s", buf);
            }
        }
    }

    return 0;
}