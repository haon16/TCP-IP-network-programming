//2020年8月31日 10:36:05
//基于半关闭的文件传输程序
//Linux下的close函数和Window下的closesocket函数是完全断开，完全断开不仅无法传输数据，而且也不能接受数据，有可能会导致一些必要的数据没有接收到

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
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen("1.file_server.c", "rb");            //open 是系统调用 返回的是文件句柄，文件的句柄是文件在文件描述符表里的索引，fopen是C的库函数，返回的是一个指向文件结构的指针。 
    if (fp == NULL)
        error_handling("fopen() error");

    int serv_sd = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sd == -1)
        error_handling("socket() error");

    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(atoi(argv[1]));
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serv_sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    
    if (listen(serv_sd, 5) == -1)
        error_handling("listen() error");

    struct sockaddr_in clnt_adr;
    socklen_t clnt_adr_sz = sizeof(clnt_adr);
    int clnt_sd = accept(serv_sd, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
    if (clnt_sd == -1)
        error_handling("accept() error");

    char buf[BUF_SIZE];
    int read_cnt;
    while (1)
    {
        read_cnt = fread(buf, 1, BUF_SIZE, fp);         //从文件流fp中读取BUF_SIZE个大小为1个字节的元素存入buf所指向的内存块（最小尺寸1*BUF_SIZE）中
        if (read_cnt < BUF_SIZE)                        //返回成功读取的元素总数，如果总数与BUF_SIZE不同，则可能发生了一个错误或者到达了文件末尾
        {
            write(clnt_sd, buf, read_cnt);
            break;
        }
        write(clnt_sd, buf, BUF_SIZE);
    }

    shutdown(clnt_sd, SHUT_WR);                         //发送文件后针对输出流进行半关闭，这样就向客户端传输了EOF，而客户端也知道文件传输已完成
    read(clnt_sd, buf, BUF_SIZE);                       //只关闭了输出流，依然可以通过输入流接收数据。客户端要发完整的字符串，包括'\0'
    printf("Message from client: %s \n", buf);

    fclose(fp);
    close(clnt_sd);
    close(serv_sd); 
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}