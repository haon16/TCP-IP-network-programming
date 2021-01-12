//时间：2020年8月26日 17:13:15
//与hello_server.c一致，这边用来验证TCP套接字的特性：传输的数据不存在数据边界

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Usage ：%s <port>\n", argv[0]);
		exit(1);
	}

	int serv_sock = socket(PF_INET, SOCK_STREAM, 0);    //IPv4协议族中面向连接的套接字，目前只有IPPROTO_TCP,所以可以填0（或者IPPROTO_TCP）
	if (serv_sock == -1)
		error_handling("socket() error");

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[1]));
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");

	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size = sizeof(clnt_addr);
	int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
	if (clnt_sock == -1)
		error_handling("accept() error");

	char message[] = "Hello World";
	write(clnt_sock, message, sizeof(message));
	close(clnt_sock);
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);       //puts() 只能向标准输出流输出，而 fputs() 可以向任何流输出。
	fputc("\n", stderr);          //使用 puts() 时，系统会在自动在其后添加换行符；而使用 fputs() 时，系统不会自动添加换行符。
	exit(1);
}