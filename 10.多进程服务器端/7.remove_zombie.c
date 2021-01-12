//2020年9月10日 10:48:36
//利用信号处理技术消灭僵尸进程

#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void read_childproc(int sig)
{
    int status;
    pid_t id = waitpid(-1, &status, WNOHANG);   //这边waitpid条件设置成WNOHANG不会出现返回0的情况，因为只有子进程结束了才会调用read_childproc函数，所以不会出现没有子进程终止的情况
    if (WIFEXITED(status))
    {
        printf("Removed proc id: %d \n", id);
        printf("Child send: %d \n", WEXITSTATUS(status));
    }
}

int main(int argc, char *argv[])
{
    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);

    pid_t pid = fork();
    if (pid == 0)       //子进程执行区域
    {
        puts("Hi! I'm child process");
        sleep(10);
        return 12;
    }
    else       //父进程执行区域 
    {
        printf("Child proc 1 id: %d\n", pid);
        pid = fork();
        if (pid == 0)     //另一子进程执行区域
        {
            puts("Hi! I'm another child process");
            sleep(10);
            exit(24);
        }
        else
        {
            printf("Child proc 2 id: %d \n", pid);
            for (int i = 0; i < 5; i++)
            {
                puts("wait...");
                sleep(5);
            }
        }
    }
    return 0;
}