//2020年9月1日 11:44:07
//读取创建套接字时默认的I/O缓冲大小:SO_SNDBUF, SO_RCVBUF

/*
创建套接字将同时生成I/O缓冲
SO_RCVBUF是输入缓冲大小相关可选项，SO_SNDBUF是输出缓冲大小相关可选项
用这2个可选项既可以读取当前I/O缓冲大小，也可以进行更改
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int snd_buf, rcv_buf;
    socklen_t len = sizeof(snd_buf);
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    int state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&snd_buf, &len);
    if (state)
        error_handling("getsockopt() error");

    len = sizeof(rcv_buf);
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&rcv_buf, &len);
    if (state)
        error_handling("getsockopt() error");
    
    printf("Input buffer size: %d\n", rcv_buf);
    printf("Output buffer size: %d\n", snd_buf);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}