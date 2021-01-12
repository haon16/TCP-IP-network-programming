//2020年9月18日 09:39:19
//利用fileno函数转换为文件描述符

#include <stdio.h>
#include <fcntl.h>

int main()
{
    int fd = open("data.dat", O_WRONLY|O_CREAT|O_TRUNC);
    if (fd == -1)
    {
        fputs("file open error", stdout);
        return -1;
    }

    printf("First file descriptor: %d \n", fd);
    FILE *fp = fdopen(fd, "w");                                 //文件描述符转换为FILE指针
    fputs("TCP/IP SOCKET PROGRAMMING \n", fp);
    printf("Second file descriptor: %d \n", fileno(fp));        //FILE指针转换为文件描述符

    fclose(fp);
    return 0;
}