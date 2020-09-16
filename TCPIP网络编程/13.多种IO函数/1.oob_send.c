//2020年9月15日 10:26:37
//介绍linux平台下的send & recv函数

/*
ssize_t send(int sockfd, const void *buf, size_t nbytes, int flags);
ssize_t recv(int sockfd, const void *buf, size_t nbytes, int flags);

flags：首发数据时的可选项，可利用位或同事传递多个信息
MSG_OOB:用于传输带外数据
*/

//MSG_OOB:发送紧急消息

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in recv_adr;
    memset(&recv_adr, 0, sizeof(recv_adr));
    recv_adr.sin_family = AF_INET;
    recv_adr.sin_port = htons(atoi(argv[2]));
    recv_adr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sock, (struct sockaddr*)&recv_adr, sizeof(recv_adr)) == -1)
        error_handling("connect() error");

    write(sock, "123", strlen("123"));
    send(sock, "4", strlen("4"), MSG_OOB);
    write(sock, "567", strlen("567"));
    send(sock, "890", strlen("890"), MSG_OOB);

    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}