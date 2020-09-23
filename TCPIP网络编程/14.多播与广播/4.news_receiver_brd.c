//2020年9月16日 20:34:13
//实现广播数据的Receiver

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
    if (argc != 2)
    {
        printf("Usage : %s <PORT>\n", argv[0]);
        exit(1);
    }

    int recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in adr;
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    adr.sin_port = htons(atoi(argv[1]));

    if (bind(recv_sock, (struct sockaddr*)&adr, sizeof(adr)) == -1)
        error_handling("bind() error");

    char buf[BUF_SIZE];
    while (1)
    {
        puts("while");
        int str_len = recvfrom(recv_sock, buf, BUF_SIZE-1, 0, NULL, 0);
        printf("str_len : %d\n", str_len);
        if (str_len < 0)
            break;
        buf[str_len] = 0;
        fputs(buf, stdout);
    }

    close(recv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}