//2020年8月28日 11:35:44
//计算器客户端

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define RLT_SIZE 4              //返回的运算结果保存所需的字节数
#define OPSZ 4                  //待算数字的字节数
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
        puts("Connected...........");

    char opmsg[BUF_SIZE];
    int result, i, opnd_cnt; 

    fputs("Operand count: ", stdout);
    scanf("%d", &opnd_cnt);
    opmsg[0] = (char)opnd_cnt;                      //待算数个数以1字节整数型传递

    for (i = 0; i < opnd_cnt; i++)
    {
        printf("Operand %d: ", i+1);
        scanf("%d", (int *)&opmsg[OPSZ*i+1]);       //int型数据保存至char数组中
    }
    fgetc(stdin);                                   //fgetc函数删掉缓冲区中的字符\n
    fputs("Operator: ", stdout);
    scanf("%c", &opmsg[opnd_cnt*OPSZ+1]);
    write(sock, opmsg, opnd_cnt*OPSZ+2);
    read(sock, &result, RLT_SIZE);

    printf("Operation result: %d \n", result);
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}