//2020年8月31日 15:19:24
//利用域名获取IP地址

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <addr>\n", argv[0]);
        exit(1);
    }

    /*
        struct hostent
        {
        char *h_name;			Official name of host.
        char **h_aliases;		Alias list. 
        int h_addrtype;		    Host address type. 
        int h_length;			Length of address. 
        char **h_addr_list;		List of addresses from name server.    IPv4和IPv6通用，这是在void指针标准化之前定义的，其实用void*也行
        }
    */
    struct hostent *host = gethostbyname(argv[1]);
    if (!host)
        error_handling("gethostbyname() error");

    printf("Official name: %s \n", host->h_name);

    for (int i = 0; host->h_aliases[i]; i++)
        printf("Aliases %d: %s \n", i+1, host->h_aliases[i]);

    printf("Address type: %s \n", (host->h_addrtype==AF_INET)?"AF_INET":"AF_INET6");

    for (int i = 0; host->h_addr_list[i]; i++)
        printf("IP addr %d: %s \n", i+1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));  //host->h_addr_list[i]先得到IP存放处的首地址信息，然后强转成in_addr解引用得到整数型地址信息，最后转换成字符串形式

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc("\n", stderr);
    exit(1);
}