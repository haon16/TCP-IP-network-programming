//2020年9月25日 11:46:28
//基于epoll的回声服务器端

/*
第12章的基于select的I/O复用服务器端中不合理的设计
1.调用select函数后常见的针对所有文件描述符的循环语句
2.每次调用select函数时都需要向该函数传递监视对象信息

调用select函数后，并不是把发生变化的文件描述符单独集中在一起，而是通过观察作为监视对象的fd_set变量的变化，
找出发生变化的文件描述符，因此无法避免针对所有监视对象的循环语句。而且，作为监视对象的fd_set变量会发生变化，
所以调用select函数前应复制并保存原有信息，并在每次调用select函数时传递新的监视对象信息

提高性能的最大障碍：相比于循环语句，更大的障碍是每次传递监视对象信息：每次调用select函数时向操作系统传递监视对象信息
                  应用程序向操作系统传递数据将对程序造成很大负担，而且无法通过优化代码解决，因此将成为性能上的致命弱点

select是监视套接字变化的函数，而套接字是由操作系统管理的，所以select函数绝对需要借助于操作系统才能完成功能。
select函数的这一缺点的弥补方式：仅向操作系统传递1次监视对象，监视范围或内容发生变化时只通知发生变化的事项
Linux : epoll   Windows : IOCP
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 100
#define EPOLL_SIZE 50
void error_handling(char *message);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(atoi(argv[1]));
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    printf("server socket: %d\n", serv_sock);

    //epoll_ctl(A, EPOLL_CTL_ADD, B, C) : epoll历程A中注册文件描述符B,主要目的是监视参数C中的事件

    int epfd = epoll_create(EPOLL_SIZE);                        //epoll方式下由操作系统负责保存监视对象文件描述符，需要向操作系统请求创建保存epoll文件描述符的空间   （select方式中是直接声明fd_set变量）
    struct epoll_event event;
    event.events = EPOLLIN;                                     //EPOLLIN发生需要读取数据的情况时
    event.data.fd = serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);          //向空间注册或注销文件描述符    （select方式中是调用FD_SET、FD_CLR函数）

    struct epoll_event *ep_events;                              //通过epoll_event结构体将发生变化的（发生事件的）文件描述符单独集中到一起   （select方式中通过fd_set变量监视对象的状态变化/事件发生与否）
    ep_events = malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

    struct sockaddr_in clnt_adr;
    socklen_t adr_sz;
    char buf[BUF_SIZE];

    while (1)
    {
        int event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);    //与select函数类似，等待文件描述符发生变化   （select方式中是调用select函数）
        if (event_cnt == -1)                                            //参数4：等待时间（毫秒），传递-1时，一直等待直到发生事件
        {
            puts("epoll_wait() error");
            break;
        }
        
        for (int i = 0; i < event_cnt; i++)             
        {
            if (ep_events[i].data.fd == serv_sock)                     //受理连接请求
            {
                adr_sz = sizeof(clnt_adr);
                int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
                event.events = EPOLLIN;
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                printf("connected client: %d \n", clnt_sock);
            }
            else            
            {
                int str_len = read(ep_events[i].data.fd, buf, BUF_SIZE);
                if (str_len == 0)                   //收到EOF返回的是0，客户端断开连接
                {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                    close(ep_events[i].data.fd);
                    printf("closed client: %d \n", ep_events[i].data.fd);
                }
                else
                {
                    write(ep_events[i].data.fd, buf, str_len);         //回传
                }
            }
        }
    }
    close(serv_sock);
    close(epfd);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}