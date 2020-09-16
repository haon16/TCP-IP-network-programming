//2020年9月16日 15:52:47

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define __USE_MISC
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage : %s <GroupIP> <PORT>\n", argv[0]);
        exit(1);
    }

    int recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in adr;
    memset(&adr, 0, sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    adr.sin_port = htons(atoi(argv[2]));

    if (bind(recv_sock, (struct sockaddr*)&adr, sizeof(adr)) == -1)
        error_handling("bind() error");
        
    struct ip_mreq join_adr;            //初始化结构体ip_mreq变量
    join_adr.imr_multiaddr.s_addr = inet_addr(argv[1]);         //初始化多播组地址
    join_adr.imr_interface.s_addr = htonl(INADDR_ANY);          //初始化待加入主机的IP地址
    setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&join_adr, sizeof(join_adr));

    char buf[BUF_SIZE];
    while (1)
    {
        puts("while");
        int str_len = recvfrom(recv_sock, buf, BUF_SIZE-1, 0, NULL, 0);     //不需要传输数据的主机地址信息，第五第六参数分别传递NULL和0 
        printf("str_len : %d\n", str_len);
        if (str_len < 0)                //实际运行获取不到信息，阻塞在recvfrom，原因未知...
            break;
        buf[str_len] = 0;
        fputs(buf, stdout);     
    }

    close(recv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}