//2020年8月26日 15:01:10
//将数据写入文件

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void error_handling(char *message);

int main()
{
    int fd = open("data.txt", O_CREAT | O_WRONLY | O_TRUNC);   //创建空文件，并只能写，如果存在data.txt文件，则清空文件的全部数据
    if (fd == -1)
        error_handling("open() error!");
    printf("file description : %d \n", fd);

    char buf[] = "Let's go!\n";
    if (write(fd, buf, sizeof(buf)) == -1)
        error_handling("write() error!");
    close(fd);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}