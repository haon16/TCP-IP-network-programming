//2020年8月26日 15:36:13
//读取文件中的数据

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define BUF_SIZE 100

void error_handling(char *message);

int main()
{
    int fd = open("data.txt", O_RDONLY);
    if (fd == -1)
        error_handling("open() error!");
    printf("file description : %d \n", fd);

    char buf[BUF_SIZE];
    if (read(fd, buf, sizeof(buf)) == -1)
        error_handling("read() error!");
    printf("file data : %s", buf);
    close(fd);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}