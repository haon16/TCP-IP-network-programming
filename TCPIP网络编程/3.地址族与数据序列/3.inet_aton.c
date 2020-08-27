//2020年8月27日 10:11:17
//将字符串信息转换为网络字节序的整数型

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

void error_handling(char *message);

int main(int argc ,char *argv[])
{
    char *addr = "127.232.124.79";
    struct sockaddr_in addr_inet;

    if (!inet_aton(addr, &addr_inet.sin_addr))   //inet_aton函数与inet_addr函数在功能上完全相同，利用了in_addr结构体也将字符串形式IP地址转换为32位网络字节序整数并返回
        error_handling("Conversion error");
    else
        printf("Network ordered integer addr: %#lx \n", addr_inet.sin_addr.s_addr);

    return 0;    
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}