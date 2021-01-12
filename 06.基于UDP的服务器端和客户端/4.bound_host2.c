//2020年8月29日 17:01:58
//存在数据边界的UDP套接字

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

    struct sockaddr_in you_adr;
    memset(&you_adr, 0, sizeof(you_adr));
    you_adr.sin_family = AF_INET;
    you_adr.sin_port = htons(atoi(argv[2]));
    you_adr.sin_addr.s_addr = inet_addr(argv[1]);

    char msg1[] = "Hi!";
    char msg2[] = "I'm another UDP host!";
    char msg3[] = "Nice to meet you";

    sendto(sock, msg1, sizeof(msg1), 0, (struct sockaddr*)&you_adr, sizeof(you_adr));
    sendto(sock, msg2, sizeof(msg2), 0, (struct sockaddr*)&you_adr, sizeof(you_adr));
    sendto(sock, msg3, sizeof(msg3), 0, (struct sockaddr*)&you_adr, sizeof(you_adr));

    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}