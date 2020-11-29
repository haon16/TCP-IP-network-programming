//2020年9月25日 20:31:55
//实现边缘触发的回声服务器端

/*
实现边缘触发：
1.通过errno变量验证错误原因             
2.为了完成非阻塞I/O，更改套接字特性

说明1：边缘触发方式中，接收数据时仅注册1次该事件，所以一旦发生输入相关事件，就应该读取输入缓冲中的全部数据，因此需要验证输入缓冲是否为空
        read函数返回-1，变量errno中的值为EAGAIN时，说明输入缓冲中没有数据可读
说明2：边缘触发方式下，以阻塞方式工作的read&write函数有可能引起服务器端的长时间停顿，读完数据后此时不应该卡在这个套接字阻塞着等待数据过来，而是要接着
        下一轮的事件询问
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>          //该头文件中声明了全局变量extern int errno,函数发生错误时会把相关的错误信息保存在其中

#define BUF_SIZE 4          //为了验证边缘触发的工作方式，将缓冲设置为4字节
#define EPOLL_SIZE 50
void error_handling(char *message);
void setnonblockingmode(int fd);

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
    setnonblockingmode(serv_sock);                        
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
        
        puts("return epoll_wait");                                  //观察事件发生数
        for (int i = 0; i < event_cnt; i++)             
        {
            if (ep_events[i].data.fd == serv_sock)                     //受理连接请求
            {
                adr_sz = sizeof(clnt_adr);
                int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
                setnonblockingmode(clnt_sock);
                event.events = EPOLLIN|EPOLLET;
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                printf("connected client: %d \n", clnt_sock);
            }
            else            
            {
                while (1)               //发生事件时需要读取输入缓冲中的所有数据
                {
                    int str_len = read(ep_events[i].data.fd, buf, BUF_SIZE);
                    if (str_len == 0)                               //收到EOF返回的是0，客户端断开连接
                    {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                        close(ep_events[i].data.fd);
                        printf("closed client: %d \n", ep_events[i].data.fd);
                        break;
                    }
                    else if (str_len < 0)
                    {
                        if (errno == EAGAIN)    //已读取输入缓冲中的全部数据
                            break;
                    }
                    else
                    {
                        write(ep_events[i].data.fd, buf, str_len);         //回传
                    }
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

void setnonblockingmode(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag|O_NONBLOCK);
}


/*
边缘触发相对于条件触发的优点：可以分离接收数据和处理数据的时间点
说明：输入缓冲区接收到数据后，对于条件触发来说，如果不读取（延迟处理），每次调用epoll_wait函数就会一直产生事件
     而对于边缘触发，服务器可以决定读取和处理的时间点，这样给服务器端的实现带来巨大的灵活性
*/