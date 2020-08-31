//2020年8月27日 09:35:21
//字节序转换

#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    unsigned short host_port = 0x1234;     //若运行程序的CPU不同，则保存的字节序也不同
    unsigned long host_addr = 0x12345678;

    unsigned short net_port = htons(host_port);          //网络字节序是大端序，如果运行环境是小端序CPU,保存的字节序有所改变(Intel和AMD系列的CPU都采用小端序标准)
    unsigned long net_addr = htonl(host_addr);

    printf("Host ordered port: %#x \n", host_port);
    printf("Network ordered port: %#x \n", net_port);
    printf("Host ordered address: %lx \n", host_addr);
    printf("Network ordered address: %lx \n", net_addr);

    return 0; 
}