//2020年8月27日 10:22:34
//inet_ntoa:此函数可以把网络字节序整数型IP地址转换成我们熟悉的字符串形式

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in addr1, addr2;
    addr1.sin_addr.s_addr = htonl(0x1020304);
    addr2.sin_addr.s_addr = htonl(0x1010101);

    char str_arr[20];
    char *str_ptr = inet_ntoa(addr1.sin_addr);       //返回值类型为char指针 返回字符串地址意味着字符串以保存到内存空间，但该函数未向程序员要求分配内存，而是在内部申请了内存并保存了字符串
    strcpy(str_arr, str_ptr);
    printf("Dotted-Decimal notation1: %s \n", str_ptr);
    
    inet_ntoa(addr2.sin_addr);
    printf("Dotted-Decimal notation2: %s \n", str_ptr);    //再次调用inet_ntoa会覆盖先前的IP地址字符串
    printf("Dotted-Decimal notation3: %s \n", str_arr);    

    return 0;
}