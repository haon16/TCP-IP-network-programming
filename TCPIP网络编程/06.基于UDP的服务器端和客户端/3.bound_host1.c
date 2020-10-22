//2020年8月29日 16:46:21
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
    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    struct sockaddr_in my_adr;
    memset(&my_adr, 0, sizeof(my_adr));
    my_adr.sin_family = AF_INET;
    my_adr.sin_port = htons(atoi(argv[1]));
    my_adr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr*)&my_adr, sizeof(my_adr)) == -1)
        error_handling("bind() error");
    
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in your_adr;
    socklen_t adr_sz;
    for (int i = 0; i < 3; i++)
    {
        sleep(5);
        adr_sz = sizeof(your_adr);                                                                  //recvfrom是阻塞的，如果没有收到消息会程序会阻塞，直到有消息过来
        str_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*)&your_adr, &adr_sz);       //recvfrom函数调用间隔为5秒，在调用前bound_host2已经调用了3次sendto函数传输数据，此时数据已经传输到bound_host1.c，
        printf("Message %d: %s \n", i+1, message);                                                  //如果是TCP程序，此时调用1次输入函数即可读入全部数据，UDP则不同，在这种情况下也需要调用3次recvfrom函数
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