//2020年9月18日 11:15:33
//基于套接字的标准I/O函数使用  --回声客户端，基于第4章的echo_client.c修改

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
                                                                               
    if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error");                                 
    else
        puts("Connected..........");

    char message[BUF_SIZE];
    int str_len;
    FILE *readfp = fdopen(sock, "r");
    FILE *writefp = fdopen(sock, "w");
    while (1)
    {
        fputs("Input message(Q to quit): ", stdout);                 
        fgets(message, BUF_SIZE, stdin);

        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

        fputs(message, writefp);
        fflush(writefp);
        fgets(message, BUF_SIZE, readfp);                       //第4章的回声客户端需要将接受的数据转换为字符串（数据的尾部插入0）
        printf("Message from server: %s", message);             //这边不需要，因为使用标准I/O函数后可以按字符串单位进行数据交换
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