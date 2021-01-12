//2020年9月12日 11:28:21
//通过管道进行进程间双向通信


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    int fds[2];
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message";
    char buf[BUF_SIZE];

    pipe(fds);
    pid_t pid = fork();
    if (pid == 0)
    {
        write(fds[1], str1, sizeof(str1));
        sleep(2);                  //注释这一行后的运行情况？ 
        read(fds[0], buf, BUF_SIZE);
        printf("Child proc output: %s \n", buf);
    }
    else
    {
        read(fds[0], buf, BUF_SIZE);
        printf("Parent proc output: %s\n", buf);
        write(fds[1], str2, sizeof(str2));
        sleep(3);           //父进程结束后就会弹出命令行提示符，这边延时结束防止子进程终止前弹出命令提示符
    }
    return 0;
}


/*
注释22行后会引发运行错误，因为向管道传递数据时，先读的进程会把数据取走
数据进入管道后成为无主数据，通过read函数先读取数据的进程将得到数据，即使该进程将数据传到了管道。
在本例中，子进程将读回自己向管道发送的数据，而父进程调用read函数后将无限期等待数据进入管道
*/

