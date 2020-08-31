//2020年8月28日 18:05:28
//基于UDP的回声客户端

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(atoi(argv[2]));
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);

    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in from_adr;
    socklen_t adr_sz;
    while (1)
    {
        fputs("Insert message(q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

        sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));       //sizeof(serv_adr)：发送时对方地址信息传的是长度
        adr_sz = sizeof(from_adr);                                                                   
        str_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*)&from_adr, &adr_sz);           //&adr_sz：接收时获取地址信息这边传的是地址
        message[str_len] = 0;                                                                           //bind函数不区分TCP和UDP，在UDP程序中同样可以调用。如果调用send函数时发现尚未分配地址信息，则在首次调用sendto函数时给相应套接字自动分配IP和端口
        printf("Message from server: %s", message);                                                     //此时分配的地址一直保留到程序结束为止，因此可以用来与其他UDP套接字进行数据交换。IP用主机IP，端口号选尚未使用的任意端口号
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
