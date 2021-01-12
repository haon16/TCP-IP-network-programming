//2020年9月14日 16:36:38
//实现I/O复用服务器端

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100
void error_handling(char *message);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(atoi(argv[1]));
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    printf("server socket: %d\n", serv_sock);

    fd_set reads, cpy_reads;
    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    
    struct sockaddr_in clnt_adr;
    socklen_t adr_sz;
    struct timeval timeout;
    int fd_max = serv_sock;
    int fd_num;
    char buf[BUF_SIZE];

    while (1)
    {
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        if ((fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout)) == -1)   //读事件
            break;

        if (fd_num == 0)
        {
            continue;
        }
        
        for (int i = 0; i < fd_max+1; i++)              //查找状态发生变化的文件描述符
        {
            if (FD_ISSET(i, &cpy_reads))
            {
                if (i == serv_sock)                     //受理连接请求
                {
                    adr_sz = sizeof(clnt_adr);
                    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
                    FD_SET(clnt_sock, &reads);          //文件描述符要注册到reads，而不是临时的cpy_reads
                    if (fd_max < clnt_sock)             //更新最大文件描述符
                        fd_max = clnt_sock;
                    printf("connected client: %d \n", clnt_sock);
                }
                else            
                {
                    int str_len = read(i, buf, BUF_SIZE);
                    if (str_len == 0)                   //收到EOF返回的是0，客户端断开连接
                    {
                        FD_CLR(i, &reads);              //文件描述符从reads清除
                        close(i);
                        printf("closed client: %d \n", i);
                    }
                    else
                    {
                        write(i, buf, str_len);         //回传
                    }
                }
            }
        }
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