//2020年9月16日 20:02:22
//实现广播数据的Sender和Receiver

/*
广播在“一次性向多个主机发送数据”，这一点上与多播类似，但传输数据的范围有区别。多播即使在跨服不同网络的情况下，
只要加入多播组就能接收数据。相反，广播只能向同一网络中的主机传输数据。

广播种类：直接广播；本地广播
二者在代码实现上的差别主要在于IP地址。
直接广播的IP地址中除了网络地址外，其余主机地址全部设置为1。例如，希望向网络地址192.12.34中的所有主机传输数据时，可以向192.12.34.255传输
            换言之，可以采用直接广播的方式向特定区域内所有主机传输数据。
本地广播中使用的IP地址限定为255.255.255.255。例如，192.32.24网络中的主机向255.255.255.255传输数据时，数据将传递到192.32.24网络中的所有主机
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
    if (argc != 3)
    {
        printf("Usage : %s <Boardcast IP> <PORT>\n", argv[0]);
        exit(1);
    }

    int send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in broad_adr;
    memset(&broad_adr, 0, sizeof(broad_adr));
    broad_adr.sin_family = AF_INET;
    broad_adr.sin_addr.s_addr = inet_addr(argv[1]);
    broad_adr.sin_port = htons(atoi(argv[2]));

    int so_brd = 1;
    setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, (void *)&so_brd, sizeof(so_brd));

    FILE *fp;
    if ((fp = fopen("news.txt", "r")) == NULL)
        error_handling("fopen() error");

    char buf[BUF_SIZE];
    while (!feof(fp))
    {
        fgets(buf, BUF_SIZE, fp);
        int len = sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr*)&broad_adr, sizeof(broad_adr));
        printf("send len : %d\n", len);
        sleep(2);
    }

    close(send_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}