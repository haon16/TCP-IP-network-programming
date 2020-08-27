//2020年8月27日 09:54:55
//将字符串信息转换为网络字节序的整数型

#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    char *addr1 = "1.2.3.4";
    char *addr2 = "1.2.3.256";   //256超过1个字节表示的整数上限，错误的IP地址以验证inet_addr函数的错误检测能力

    unsigned long conv_addr = inet_addr(addr1);  //inet_addr函数将IP地址转换为网络字节序的32位整数型
    if (conv_addr == INADDR_NONE)
        printf("Error occured! \n");
    else
        printf("Network ordered integer addr: %#lx \n", conv_addr);

    conv_addr = inet_addr(addr2);
    if (conv_addr == INADDR_NONE)
        printf("Error occured \n");
    else
        printf("Network ordered integer addr: %#lx \n", conv_addr);

    return 0; 
}