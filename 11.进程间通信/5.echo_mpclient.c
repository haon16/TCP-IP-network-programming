//2020年9月11日 10:41:26
//回声客户端的I/O程序分割   同第10章，不做修改

//分隔前：客户端每次向服务器传输数据后，都得等待服务器回复，直到接受完服务器端的回声数据后，才能传输下一批数据
//分隔后：不同进程分别负责输入和输出，这样无论客户端是否从服务器端接受完数据都可以进行传输
//分隔I/O程序的另一个好处是：不必再考虑接受数据的情况，因此可以连续发送数据。可以提高频繁交换数据的程序性能。

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(atoi(argv[2]));
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error");

    char buf[BUF_SIZE];
    pid_t pid = fork();
    if (pid == 0)
        write_routine(sock, buf);     //数据输出
    else
        read_routine(sock, buf);      //数据输入
    
    close(sock);
    return 0;
}

void read_routine(int sock, char *buf)
{
    while (1)
    {
        int str_len = read(sock, buf, BUF_SIZE);
        if (str_len == 0)
            return;

        buf[str_len] = 0;
        printf("Message from server: %s", buf);
    }
}

void write_routine(int sock, char *buf)
{
    while (1)
    {
        fgets(buf, BUF_SIZE, stdin);
        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
        {
            shutdown(sock, SHUT_WR);            //调用shutdown函数向服务器端传递EOF（发送FIN）。虽说45行的close函数也能传递EOF，但是fork之后复制了文件描述符，此时无法通过1次close函数调用传递EOF，因此需要通过shutdown函数调用另外传递。
            return;                             //只有在调用close使得相应描述符引用计数为0时，才会传递EOF，而采用shutdown，则使描述符的引用计数仍然大于0但EOF也被强迫发送
        }
        write(sock, buf, strlen(buf));
    }
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}


//无论复制出多少文件描述符，均应调用shutdown函数发送EOF并进入半关闭状态
