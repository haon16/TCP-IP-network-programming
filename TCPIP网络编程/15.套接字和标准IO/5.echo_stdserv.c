//2020年9月18日 10:08:47
//基于套接字的标准I/O函数使用  --回声服务器端，基于第4章的echo_server.c修改

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
    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(atoi(argv[1]));
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    struct sockaddr_in clnt_adr;
    socklen_t clnt_adr_size = sizeof(clnt_adr);
    char message[BUF_SIZE];
    int str_len;
    FILE *readfp, *writefp;
    
    for (int i = 0; i < 5; i++)
    {
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
        if (clnt_sock == -1)
            error_handling("accept() error");
        else
            printf("Connected client %d \n", i+1);

        readfp = fdopen(clnt_sock, "r");
        writefp = fdopen(clnt_sock, "w");
        while (!feof(readfp))
        {
            fgets(message, BUF_SIZE, readfp);
            fputs(message, writefp);
            fflush(writefp);
        }
        fclose(readfp);
        fclose(writefp);
        puts("client disconnect");
    }

    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}


//while循环中调用基于字符串的fgets,fputs函数提供服务，并调用fflush函数。标准I/O函数为了提高性能，内部提供额外的缓冲。
//因此，若不调用fflush函数则无法保证立即将数据传输到客户端