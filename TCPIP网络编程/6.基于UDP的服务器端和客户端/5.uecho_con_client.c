//2020年8月29日 17:56:47
//创建已连接UDP套接字
//服务器端可以用1.uecho_erver.c，客户端基于2.uecho_client.c修改成已连接UDP套接字的程序

/*
TCP套接字中需注册待传输数据的目标IP和端口号，而UDP中无需注册。因此通过sendto函数传输数据的过程大致分为以下3个阶段
第一阶段：向UDP套接字注册目标IP和端口号
第二阶段：传输数据
第三阶段：删除UDP套接字中注册的目标地址信息
每次调用sendto函数都会重复上述过程，每次都变更目标地址，因此可以重复利用同一UDP套接字向不同目标传输数据
未注册目标地址信息的套接字称为未连接套接字 --》 UDP套接字默认属于未连接套接字  
注册了目标地址的套接字称为连接connected套接字 --》 如果要与同意主机进行长时间通信的话，将UDP套接字设置成连接套接字会提高效率
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[])
{
    int sock;
    char message[BUF_SIZE];
    int str_len;
    //socklen_t adr_sz;             //不需要了

    struct sockaddr_in serv_adr /*from_adr*/;    //不再需要from_adr
    if (argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(atoi(argv[2]));
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);

    connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));   //创建过程和TCP套接字创建过程一致，除了socket参数2是SOCK_DGRAM,针对UDP套接字调用connect函数并不意味着要与对方UDP套接字连接，这只是向UDP套接字注册目标IP和端口信息
                                                                    //之后就与TCP套接字一样，可以调用sendto,recvfrom传输数据，因为已经指定了收发对象，所以也可以使用write,read函数进行通信
    while (1)
    {
        fputs("Insert message(q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;
       
        //sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));  
        write(sock, message, strlen(message)); 

        //adr_sz = sizeof(from_adr);                                                                   
        //str_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*)&from_adr, &adr_sz);
        str_len = read(sock, message, sizeof(message)-1);

        message[str_len] = 0;                                                                           
        printf("Message from server: %s", message);                                                 
    }

    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
