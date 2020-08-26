//2020年8月26日 15:48:42
//文件描述符与套接字

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

int main()
{
    int fd1, fd2, fd3;
    fd1 = socket(PF_INET, SOCK_STREAM, 0);
    fd2 = open("test.dat", O_CREAT | O_WRONLY | O_TRUNC);
    fd3 = socket(PF_INET, SOCK_DGRAM, 0);

    printf("file description 1: %d\n", fd1);  //3
    printf("file description 2: %d\n", fd2);  //4
    printf("file description 3: %d\n", fd3);  //5

    close(fd1);
    close(fd2);
    close(fd3);
    return 0;
}


//从输出的文件描述符整数值可以看出，描述符从3开始以由小到大的顺序编号，因为0，1，2是分配给标准I/O的描述符
// 0:标准输入   1：标准输出   2：标准错误  即使未经过特殊的创建过程，程序开始运行后也会被自动分配文件描述符