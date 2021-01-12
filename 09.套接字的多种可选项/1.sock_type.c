//2020年9月1日 11:06:24
//获取套接字的类型:SO_TYPE

/*
套接字多种可选项
套接字可选项是分层的：IPPROTO_IP层可选项是IP协议相关事项：SO_SNDBUF、SO_RCVBUF、SO_KEEPALIVE、SO_REUSEADDR、SO_ERROR、SO_TYPE...
                    IPPROTO_TCP层可选项是TCP协议相关事项:IP_TTL、IP_TOS、IPMULTICAST_TTL...
                    SOL_SOCKET是套接字相关的通用可选项:TCP_KEEPALIVE、TCP_NODELAY、TCP_MAXSEG
对可选项进行读取和设置：getsockopt & setsockopt

int getsockopt (int sock, int level, int optname, void *optval, socklen_t *optlen)
成功时返回0，失败时返回-1
sock    用于查看选项套接字文件描述符
level   要查看的可选项的协议层
optname 要查看的可选项名
optval  保存查看结果的缓冲地址值
optlen  向第四个参数optval传递的缓冲大小。调用函数后，该变量中保存通过第四个参数返回的可选项信息的字节数

int setsockopt (int sock, int level, int optname, const void *optval, socklen_t *optlen)
大致跟getsockopt参数意义一样
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int tcp_sock = socket(PF_INET, SOCK_STREAM, 0);
    int udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
    printf("SOCK_STREAM: %d \n", SOCK_STREAM);
    printf("SOCK_DGRAM: %d \n", SOCK_DGRAM);

    int sock_type;
    socklen_t optlen = sizeof(sock_type);
    int state = getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen);  
    if (state)
        error_handling("getsockopt() error");
    printf("Socket type one: %d \n", sock_type);

    state = getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen);     //SO_TYPE是只读可选项，因为套接字类型只能在创建时决定，之后不能再更改
    if (state)
        error_handling("getsockopt() error");
    printf("Socket type two: %d \n", sock_type);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}