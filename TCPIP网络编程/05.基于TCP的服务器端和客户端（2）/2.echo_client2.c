//2020年8月28日 09:53:17
//回声客户端问题解决

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
    int sock;
    char message[BUF_SIZE];
    struct sockaddr_in serv_adr;
    int str_len, recv_len, recv_count;

    if (argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");
    
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(atoi(argv[2]));
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error");
    else
        puts("Connected...........");

    while (1)
    {
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;
        
        str_len = write(sock, message, strlen(message));                    //解决方法：确定发送的数据长度，限定接收的长度
                                                                            //回声客户端可以提前知道接收的数据长度，但更多情况下这不太可能，若无法预知接收数据长度，此时需要的就是应用层协议的定义
        recv_len = 0;                                                       //正如“收到Q就立即终止连接”，收发数据过程中也需要定好规则（协议）以表示数据的边界，或提前告知收发数据的大小。服务器端/客户端实现过程中逐步定义的这些规则集合就是应用层协议
        while (recv_len < str_len)
        {
            recv_count = read(sock, &message[recv_len], BUF_SIZE-1);
            if (recv_count == -1)
                error_handling("read() error");
            recv_len += recv_count;
        }
        message[recv_len] = 0;
        printf("Message from server: %s", message);
    }

    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}