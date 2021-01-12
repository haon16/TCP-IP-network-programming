//时间：2020年8月26日 17:14:23
//修改hello_client.c中read函数的调用方式以验证TCP套接字的特性：传输的数据不存在数据边界
//需要让write函数的调用次数不同于read函数的调用次数，因此在客户端中分多次调用read函数以接收服务器端发送的全部数据

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
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);    //inet_addr的参数是字符串，返回值是网络字节序，htonl的参数是32bit的ip，并且是主机字节序

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error!");

    char message[30];
    int str_len;
    int idx = 0, read_len = 0;
    while (read_len = read(sock, &message[idx++], 1))    //每次只读取1个字节
    {
        if (read_len == -1)
            error_handling("read() error!");

        str_len += read_len;
    }
    
    printf("Message from server: %s \n", message);
    printf("Function read call count: %d \n", str_len);
    close(sock);
    return 0;    
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}