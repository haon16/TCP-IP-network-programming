//2020年9月1日 15:10:46
//地址再分配:SO_REUSEADDR
//发生地址分配错误（Bingding Error），将Time-wait状态下的套接字端口号重新分配给新的套接字

/*
让客户端先通知服务器端终止程序：输入Q消息时调用close函数，向服务器端发送FIN消息并经过四次握手过程。或者输入CTRL+C时也会向服务器传递FIN消息。
强制终止程序时，由操作系统关闭文件及套接字，此过程相当于调用close函数，也会向服务器端传递FIN消息

已建立连接的状态下客户端先断开连接 -> 无异常，客户端重新运行程序和服务器端重新运行都不成问题
已建立连接的状态下服务器先断开连接（CTRL+C强制关闭服务器端） -> 服务器端如果用同一端口号重新运行的话，将输出bind() error且无法再次运行，这种情况下得过大约3分钟即可重新运行服务器端
两种运行方式的唯一的区别就是谁先传输FIN消息。

断开连接，套接字经过四次握手过程后并非立即消除，而是要经过一段时间的Time-wait状态。只有先断开连接（先发送FIN消息的）主机才经过Time-wait状态。
因此服务器端先断开连接，则无法立即重新运行。套接字处在Time-wait过程时，相应端口是正在使用的状态。
客户端套接字的端口号是任意指定的，与服务器不同，客户端每次运行程序时都会动态分配端口号，因此无需过多关注Time-wait状态

如果没有Time-wait状态：主机A给主机B发送最后的ACK消息在传递途中丢失，主机B重传FIN消息，但此时主机A已经是完全终止的状态导致B永远也无法收到主机A最后传来的ACK消息
如果有Time-wait状态主机A会向主机B重传最后的ACK消息，主机B也可以正常终止。所以先传输FIN消息的主机应经过Time-wait过程

系统故障而紧急停止时需要尽快重启服务器端以提供服务，而Time-wait状态必须等待几分钟，B重传FIN，A收到后重启Time-wait计时器，如果网络状况不理想，Time-wait状态将持续
解决方案就是在套接字可选项中更改SO_REUSEADDR的状态 默认值为0，意味着无法分配Time-wait状态下的套接字端口号，因此需要将这个值改成1
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TRUE 1
#define FALSE 0

void error_handling(char *message);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int serv_sock =socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    /*
    //端口可再分配
    int option = TRUE;
    socklen_t optlen = sizeof(option);
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void *)&option, &optlen);
    */

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
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
    if (clnt_sock == -1)
        error_handling("accept() error");

    char message[30];
    int str_len;
    while ((str_len = read(clnt_sock, message, sizeof(message))) != 0)
    {
        write(clnt_sock, message, str_len);
        write(1, message, str_len);
    }
    
    close(clnt_sock);
    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
