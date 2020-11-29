//2020年9月25日 15:49:23
//掌握条件触发的事件特性

/*
epoll中的条件触发(level trigger)和边缘触发(edge trigger):                
条件触发，只要输入缓冲有数据就会一直通知该事件（以事件方式再次注册）   
边缘触发，输入缓冲收到数据时仅注册1次该事件。即使输入缓冲中还留有数据，也不会再进行注册

epoll默认以条件触发方式工作
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 4          //跟echo_epollserv.c的差异1：调用read函数时使用的缓冲大小缩减为4个字节
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

    int epfd = epoll_create(EPOLL_SIZE);                        
    struct epoll_event event;
    event.events = EPOLLIN;                                     
    event.data.fd = serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);          

    struct epoll_event *ep_events;                              
    ep_events = malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

    struct sockaddr_in clnt_adr;
    socklen_t adr_sz;
    char buf[BUF_SIZE];

    while (1)
    {
        int event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);    
        if (event_cnt == -1)                                            
        {
            puts("epoll_wait() error");
            break;
        }
        
        puts("return epoll_wait");              //跟echo_epollserv.c的差异2：插入验证epoll_wait函数调用次数的语句
        for (int i = 0; i < event_cnt; i++)             
        {
            if (ep_events[i].data.fd == serv_sock)                     //受理连接请求
            {
                adr_sz = sizeof(clnt_adr);
                int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
                event.events = EPOLLIN|EPOLLET;
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                printf("connected client: %d \n", clnt_sock);
            }
            else            
            {
                int str_len = read(ep_events[i].data.fd, buf, BUF_SIZE);
                if (str_len == 0)                   //收到EOF返回的是0，客户端断开连接
                {
                    printf("adsadasd");
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