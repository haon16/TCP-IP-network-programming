//2020年9月29日 14:18:09
//多线程并发服务器端的实现

/*
销毁线程的三种方法
Linux线程并不是在首次调用的线程main函数返回时自动销毁，所以用如下2种方法之一加以明确。否则由线程创建的内存空间将一直存在
1.调用pthread_join函数  
2.调用pthread_detach函数
3.虽然还有方法在创建线程时可以指定销毁时机，但与pthread_detach方式相比，结果没有太大差异，省略说明

1.调用pthread_join函数时，不仅会等待线程终止，还会引导线程销毁。但线程终止前，调用该函数的线程将进入阻塞状态
2.调用pthread_detach函数不会引起线程终止或进入阻塞状态，可以通过该函数引导销毁线程创建的内存空间。调用该函数后不能再针对相应线程调用pthread_join函数
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void* handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);

int clnt_cnt = 0;               //访问这2个变量的代码将构成临界区
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);
    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    pthread_t t_id;
    struct sockaddr_in clnt_adr;
    int clnt_adr_sz;
    while (1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutx);

        pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_sock);
        pthread_detach(t_id);
        printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
    }

    close(serv_sock);
    return 0;
}

void* handle_clnt(void *arg)
{
    int clnt_sock = *((int *)arg);
    int str_len = 0;
    char msg[BUF_SIZE];

    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
        send_msg(msg, str_len);

    pthread_mutex_lock(&mutx);
    for (int i = 0; i < clnt_cnt; i++)          //移除断开连接的客户端socket
    {
        if (clnt_sock == clnt_socks[i])
        {
            while (i++ < clnt_cnt-1)
                clnt_socks[i] = clnt_socks[i+1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);

    close(clnt_sock);
    return NULL;
}

void send_msg(char *msg, int len)       //发送给所有客户端
{
    pthread_mutex_lock(&mutx);
    for (int i = 0; i < clnt_cnt; i++)
        write(clnt_socks[i], msg, len);
    pthread_mutex_unlock(&mutx);
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

/*
添加或删除客户端时，变量clnt_cnt和数组clnt_socks同时发生变化。在如下情形中均会导致数据不一致，从而引发严重错误
1.线程A从数组clnt_socks中删除套接字信息，同时线程B读取clnt_cnt变量
2.线程A读取变量clnt_cnt，同时线程B将套接字信息添加到clnt_socks数组
因此，访问（值的更改）变量clnt_cnt和数组clnt_socks的代码应组织在一起并构成临界区
*/