//2020年9月10日 14:26:49
//实现基于进程的并发服务器

#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_childproc(int sig);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);

    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(atoi(argv[1]));
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if (listen(serv_sock, 5) == - 1)
        error_handling("listen() error");
    
    struct sockaddr clnt_adr;
    int str_len;
    char buf[BUF_SIZE];
    while (1)
    {
        int adr_sz = sizeof(clnt_adr);
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
        if (clnt_sock == -1)
            continue;
        else
            puts("new client connected...");
        
        int pid = fork();
        if (pid == -1)
        {
            close(clnt_sock);
            continue;
        }
        if (pid == 0)           //子进程运行区域
        {
            close(serv_sock);           //关闭同样传递到子进程的服务器套接字文件描述符
            while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0)
                write(clnt_sock, buf, str_len);

            close(clnt_sock);
            puts("client disconnected...");
            return 0;
        }
        else
            close(clnt_sock);           //accept函数创建的套接字文件描述符已复制给子进程，这边服务器需要销毁自己拥有的文件描述符                     
    }                               
    close(serv_sock);
    return 0;
}

void read_childproc(int sig)
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("remove proc id: %d \n", pid);
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

/*
通过fork函数复制文件描述符，fork之后父进程将2个套接字（一个是服务器端套接字，另一个是与客户端连接的套接字）文件描述符复制给子进程
调用fork函数时复制父进程的所有资源，有些人可能认为也会同时复制套接字。但套接字并非进程所有，严格意义上是属于操作系统，只是进程拥有代表相应套接字的文件描述符
也不一定非要这样理解，仅因为如下原因，复制套接字也并不合理     “复制套接字后，统一端口将对应多个套接字”

                                指向     操作系统
    fork后     父进程 serv_sock   ->   服务器端套接字
               父进程 clnt_sock   ->   客户端连接套接字

               子进程 serv_sock   ->   服务器端套接字
               子进程 clnt_sock   ->   客户端连接套接字
        
        1个套接字中存在2个文件描述符，只有2个文件描述符都终止（销毁）后，才能销毁套接字，如果维持上面的连接状态，即使子进程销毁了与客户端连接的套接字文件描述符
        也无法完全销毁套接字（服务器端套接字同样如此），因此调用fork函数后，要将无关的套接字文件描述符关掉
*/