//2020年9月17日 10:25:42
//系统函数复制文件

/*
标准I/O函数和系统函数之间的性能对比
标准I/O函数的优点：1.具有良好的移植性 2.可以利用缓冲提高性能

缓冲的关系：
            I/O函数缓冲      套接字缓冲
       <--     数据      <--   数据      <--
            （输入缓冲）     （输入缓冲）
本地                                            网络
       -->     数据      -->   数据      -->
            （输出缓冲）     （输出缓冲）
使用标准I/O函数传输数据时。经过2个缓冲。例如，通过fputs函数传输字符串“Hello”时，首先将数据传递到I/O函数的缓冲。然后数据
将移动到套接字输出缓冲，最后将字符串发送到对方主机

I/O函数缓冲的主要目的：提高性能    -->传输的数据量（1个字节的数据发送10次需要10个数据包，累计10个字节只需要发送1次）；数据向输出缓冲移动的次数（发送数据向套接字输出缓冲移动数据，1字节数据移动10次和10个字节移动1次）
套接字中缓冲的主要目的：为了实现TCP协议（比如丢失数据时要再重传，数据是保存在套接字缓冲中）
*/

#include <stdio.h>
#include <fcntl.h>
#include <time.h>

#define BUF_SIZE 3   //用最短数组长度构成

int main(int argc, char *argv[])
{
    int len;
    char buf[BUF_SIZE];

    int fd1 = open("news.txt", O_RDONLY);
    int fd2 = open("cpy.txt", O_WRONLY|O_CREAT|O_TRUNC);   

    clock_t cbegin = clock();
    printf("cbegin : %d\n", cbegin);

    while ((len = read(fd1, buf, sizeof(buf))) > 0)
        write(fd2, buf, len);

    clock_t cend = clock();
    printf("cend : %d\n", cend);
    printf("diffsec : %d\n", (cend-cbegin)/CLOCKS_PER_SEC);   //clock()换算成秒数要除以CLOCKS_PER_SEC，windows下是1000，linux下是1000000 

    close(fd1);
    close(fd2);
    return 0;
}