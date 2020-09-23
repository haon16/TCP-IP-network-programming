//2020年9月18日 09:18:30
//利用fdopen函数转换为FILE结构体指针

//创建套接字时返回文件描述符，而为了使用标准I/O函数，只能将其转换为FILE结构体指针

#include <stdio.h>
#include <fcntl.h>

int main()
{
    int fd = open("data.dat", O_WRONLY|O_CREAT|O_TRUNC);    //使用open函数创建文件并返回文件描述符
    if (fd == -1)
    {
        fputs("file open error", stdout);
        return -1;
    }

    FILE *fp = fdopen(fd, "w");                 //调用fdopen函数将文件描述符转换为FILE指针，第二个参数传递了“w”,返回写模式的FILE指针
    fputs("Network C programming \n", fp);

    fclose(fp);                 //利用FILE指针关闭文件，此时完全关闭，因此无需再通过文件描述符关闭。而且调用fclose函数后，文件描述符也变成毫无意义的整数
    return 0;                  
}