//2020年9月20日 17:21:56
//创建2个管道，各自负责不同的数据流动，完成双向通信

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    int fds1[2], fds2[2];
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message";
    char buf[BUF_SIZE];

    pipe(fds1);
    pipe(fds2);
    pid_t pid = fork();
    if (pid == 0)
    {
        write(fds1[1], str1, sizeof(str1));
        read(fds2[0], buf, BUF_SIZE);
        printf("Child proc output: %s \n", buf);
    }
    else
    {
        read(fds1[0], buf, BUF_SIZE);
        printf("Parent proc output: %s \n", buf);
        write(fds2[1], str2, sizeof(str2));
        sleep(3);            //延迟命令行提示符提示
    }
    
    return 0;
}