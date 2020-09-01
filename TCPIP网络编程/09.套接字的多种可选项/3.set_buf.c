//2020年9月1日 14:25:36
//更改套接字默认的I/O缓冲大小:SO_SNDBUF,SO_RCVBUF

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int snd_buf = 1024*3, rcv_buf = 1024*3;             //I/O缓冲大小更改为3M字节
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    int state = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&rcv_buf, sizeof(rcv_buf));
    if (state)
        error_handling("setsockopt() error");

    state = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&snd_buf, sizeof(snd_buf));
    if (state)
        error_handling("setsockopt() error");

    socklen_t len = sizeof(snd_buf);
    state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&snd_buf, &len);
    if (state)
        error_handling("getsockopt() error");
    
    len = sizeof(rcv_buf);
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&rcv_buf, &len);
    if (state)
        error_handling("getsockopt() error");
    
    printf("Input buffer size: %d \n", rcv_buf);      //更改后打印出来的I/O缓冲大小都是6144字节
    printf("Output buffer size: %d \n", snd_buf);     //缓冲大小的设置需谨慎处理，因此不会完全按照我们的要求进行，只是通过调用setsockopt函数向系统传递我们的要求

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
