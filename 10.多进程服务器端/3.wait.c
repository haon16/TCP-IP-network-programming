//2020年9月3日 16:20:45
//销毁僵尸进程1：利用wait函数
//ps:调用wait函数时，如果没有已终止的子进程，那么程序将阻塞直到有子进程终止，谨慎调用

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int status;
    pid_t pid = fork();

    if (pid == 0)
    {
        return 3;
    }
    else
    {
        printf("Child PID: %d \n", pid);
        pid = fork();
        if (pid == 0)
        {
            exit(7);
        }
        else
        {
            printf("Child PID: %d \n", pid);
            wait(&status);                                                  //之前终止的子进程相关信息将保存到status变量，同时相关子进程被完全销毁
            if (WIFEXITED(status))                                          //WIFEXITED:子进程正常终止时返回true
                printf("Child send one: %d \n", WEXITSTATUS(status));       //WEXITSTATUS:返回子进程的返回值

            wait(&status);                                                  
            if (WIFEXITED(status))
                printf("Child send two: %d \n", WEXITSTATUS(status));
            sleep(30);
        }
    }

    return 0;
}