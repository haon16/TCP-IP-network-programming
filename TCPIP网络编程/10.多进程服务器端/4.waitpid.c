//2020年9月3日 16:50:11
//销毁僵尸进程2：使用waitpid函数
//waitpid函数是防止僵尸进程的第二种方法，也是防止阻塞的方法

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    int status;
    pid_t pid = fork();

    if (pid == 0)
    {
        sleep(15);
        return 24;
    }
    else
    {
        /*  pid_t waitpid(pid_t pid, int *statloc, int options)
            pid     : 等待终止的目标子进程ID，若传递-1，则与wait函数相同，可以等待任意子进程终止
            statloc : 与wait函数的statloc参数具有相同含义
            options : 传递头文件sys/wait.h中声明的常量WNOHANG,即使没有终止的子进程也不会进入阻塞状态，而是返回0并退出函数
            返回值  : 成功时返回终止的子进程ID（或0），失败时返回-1
        */
        while (!waitpid(-1, &status, WNOHANG))    
        {
            sleep(3);
            puts("sleep 3sec.");
        }

        if (WIFEXITED(status))
            printf("Child send %d \n", WEXITSTATUS(status));
    }

    return 0;
}