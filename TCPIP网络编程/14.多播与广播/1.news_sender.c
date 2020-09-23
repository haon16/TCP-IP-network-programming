//2020年9月16日 14:14:31
//实现多播Sender

/*
多播的数据传输特点：
1.多播服务器端针对特定多播组，只发送1次数据
2.即使只发送1次数据，但该组内的所有客户端都会接收数据
3.多播组数可在IP地址范围内任意增加
4.加入特定组即可接收发往该多播组的数据

多播组是D类IP地址（224.0.0.0~239.255.255.255）
多播是基于UDP完成，多播数据包的格式与UDP数据包相同。只是与一般UDP数据包不同，向网络传递1个多播数据包时，路由器将复制该数据包并传递到多个主机。
若通过TCP或UDP向1000个主机发送文件，则共需要传递1000次。若使用多播方式传输文件，则只需要发送1次。
这时由1000台主机构成的网络中的路由器负责复制文件并传递到主机，因为这种特性，多播主要用于“多媒体数据的实时传输”
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TTL 64
#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage : %s <GroupIP> <PORT> \n", argv[0]);
        exit(1);
    }

    int send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    int time_live = TTL;
    setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&time_live, sizeof(time_live));      //必须指定套接字TTL信息，不可能让路由器无限传送
    
    struct sockaddr_in mul_adr;                     //设置传输数据的目标地址信息，必须将IP地址设置为多播地址
    memset(&mul_adr, 0, sizeof(mul_adr));
    mul_adr.sin_family = AF_INET;
    mul_adr.sin_addr.s_addr = inet_addr(argv[1]);   //Multicast IP
    mul_adr.sin_port = htons(atoi(argv[2]));        //Multicast Port

    FILE *fp;
    if ((fp = fopen("news.txt", "r")) == NULL)
        error_handling("fopen() error");

    char buf[BUF_SIZE];      //Broadcasting
    while(!feof(fp))
    {
        fgets(buf, BUF_SIZE, fp);
        int len = sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr*)&mul_adr, sizeof(mul_adr));
        printf("send len : %d\n", len);
        sleep(2);           //给传输数据提供一定的时间间隔，无其他特殊意义
    }

    fclose(fp);
    close(send_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}