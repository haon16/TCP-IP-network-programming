//时间：2020年8月26日 14:09:00
//构建打电话套接字

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error!");
    
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);                 //inet_addr的参数是字符串，返回值是网络字节序，htonl的参数是32bit的ip，并且是主机字节序

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)       //调用connect函数后，发生以下情况之一才会返回：服务器端接收连接请求；发生断网等异常情况而中断连接请求
        error_handling("connect() error!");                                         //“接收连接”并不意味着服务器端调用accept函数，其实是服务器端把连接请求信息记录到等待队列。因此connect函数返回后并不立即进行数据交换

    char message[30];
    int str_len = read(sock, message, sizeof(message)-1);
    if (str_len == -1)
        error_handling("read() error!");

    printf("Message from server : %s \n", message);
    close(sock);
    return 0;    
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}