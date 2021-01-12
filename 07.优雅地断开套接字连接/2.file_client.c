//2020年8月31日 11:46:13
//基于半关闭的文件传输程序

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
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen("receice_data", "wb");
    if (fp == NULL)
        error_handling("fopen() error");

    int sd = socket(PF_INET, SOCK_STREAM, 0);
    if (sd == -1)
        error_handling("socket() error");
    
    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(atoi(argv[2]));
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error");

    char buf[BUF_SIZE];
    int read_cnt;
    while ((read_cnt = read(sd, buf, BUF_SIZE)) != 0)     //接受数据并保存到receive_data中，直到接收EOF为止
        fwrite((void*)buf, 1, read_cnt, fp);

    puts("Receive file data");
    write(sd, "Thank you", 10);            //向服务器端发送表示感谢的消息。如果服务器端未关闭输入流，则可接收此消息
    fclose(fp);
    close(sd);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}