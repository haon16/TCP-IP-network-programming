//2020年9月15日 10:50:55
//MSG_OOB:接收紧急消息

#define _XOPEN_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define BUF_SIZE 30
void error_handling(char *message);
void urg_handler(int signo);

int serv_sock;
int clnt_sock;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    struct sigaction act;
    act.sa_handler = urg_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(atoi(argv[1]));
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    struct sockaddr_in clnt_adr;
    socklen_t clnt_adr_sz = sizeof(clnt_adr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

    fcntl(clnt_sock, __F_SETOWN, getpid());         //文件描述符clnt_sock指向的套接字引发的SIGURG信号处理进程变为将getpid函数返回值用作ID的进程
                                                    //处理SIGURG信号时必须制定处理信号的进程（因为存在多进程情况，不会每个进程都调用信号处理函数），
                                                    //而getpid函数就是返回调用此函数的进程ID,本例中制定了当前进程为处理SIGURG信号的主体
    
    int state = sigaction(SIGURG, &act, 0);         //收到MSG_OOB紧急消息时，操作系统将产生SIGURG信号，并调用注册的信号处理函数

    int str_len;
    char buf[BUF_SIZE];
    while ((str_len = recv(clnt_sock, buf, sizeof(buf), 0)) != 0)
    {
        if (str_len == -1)
            continue;
        buf[str_len] = 0;
        puts(buf);
    }

    close(clnt_sock);
    close(serv_sock);
    return 0;
}

void urg_handler(int signo)
{
    char buf[BUF_SIZE];
    int str_len = recv(clnt_sock, buf, sizeof(buf)-1, MSG_OOB);
    buf[str_len] = 0;
    printf("Urgent message: %s \n", buf);
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

/*
输出结果跟书中不一样：
Urgent message: 0 
12356789 

书：
123
Urgent message: 4
567
Urgent message: 0
89

通过MSG_OOB可选项传递数据时不会加快数据传输速度，而且通过信号处理函数urg_handler读取数据时也只能读1个字节。剩余数据只能通过未设置MSG_OOB可选项的普通
输入函数读取。这是因为TCP不存在真正意义上的“带外数据”。实际上MSG_OOB中的OOB是指Out-of-band，而“带外数据”的含义是“通过完全不同的通信路径传输的数据”
即真正意义上的Out-of-band需要通过单独的通信路径高速传输数据，但TCP不另外提供，只利用TCP的紧急模式进行传输

紧急模式工作原理：
MSG_OOB的真正意义在于督促数据接收对象尽快处理数据，这是紧急模式的全部内容，而且TCP“保持传输顺序”的传输特性依然成立
 
TCP数据包： URG=1，载有紧急消息的数据包；URG指针，紧急数据在数据包中的偏移量
指定MSG_OOB选项的数据包本身就是紧急数据包，并通过紧急指针表示紧急消息所在位置。
但是无法得知：紧急消息是字符串890，还是90，还是单个字符0？
但这并不重要，除紧急指针的前面1个字节外，数据接收方将通过调用常用输入函数读取剩余部分
换言之，紧急消息的意义在于督促消息处理，而非紧急传输形式受限的消息
*/