//2020年8月27日 17:10:54
//迭代回声客户端

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
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
        error_handling("socket() error");

    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(atoi(argv[2]));
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
                                                                               //客户端调用connect函数时分配IP和端口给套接字
    if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)    //若调用connect函数引起的连接请求被注册到服务器端等待队列，则connect函数将完成调用。因此即使
        error_handling("connect() error");                                     //输出了连接提示字符串，如果服务器尚未调用accept函数，也不会真正建立服务关系
    else
        puts("Connected..........");

    char message[BUF_SIZE];
    int str_len;
    while (1)
    {
        fputs("Input message(Q to quit): ", stdout);                 
        fgets(message, BUF_SIZE, stdin);

        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

        write(sock, message, strlen(message));                  //存在的问题：1.多次调用的write函数传递的字符串有可能一次性传递到服务端 2.服务器端调用1次write函数传输数据，如果数据太大，操作系统有可能把数据分成多个数据包发送到客户端，在此过程中，客户端有可能尚未收到全部数据包时就调用read函数
        str_len = read(sock, message, BUF_SIZE-1);              //原因：TCP不存在数据边界
        message[str_len] = 0;
        printf("Message from server: %s", message);   
    }

    close(sock);            //调用close函数向相应套接字发送EOF（EOF即意味着中断连接）
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}