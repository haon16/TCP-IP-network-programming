//2020年8月31日 16:07:07
//利用IP地址获取域名

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int i;
    struct hostent *host;
    struct sockaddr_in addr;
    /*
        struct hostent
        {
        char *h_name;			Official name of host.
        char **h_aliases;		Alias list. 
        int h_addrtype;		    Host address type. 
        int h_length;			Length of address. 
        char **h_addr_list;		List of addresses from name server. 
        }
    */

    if (argc != 2)
    {
        printf("Usage: %s <IP>\n", argv[0]);
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    host = gethostbyaddr((void*)&addr.sin_addr, 4, AF_INET);    //IPv4：4个字节，AF_INET   
    if (!host)                                                  //测试时要etc/hosts中手动增加一个映射，不然会返回空。 网上查大致是说使用这个函数本地要有反向解析的服务，不太确定
        error_handling("gethostbyaddr() error");

    printf("Official name: %s \n", host->h_name);

    for (i = 0; host->h_aliases[i]; i++)
        printf("Aliases %d: %s \n", i+1, host->h_aliases[i]);

    printf("Address type: %s \n", (host->h_addrtype==AF_INET)?"AF_INET":"AF_INET6");

    for (i = 0; host->h_addr_list[i]; i++)
        printf("IP addr %d: %s \n", i+1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));

    return 0;        
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
} 