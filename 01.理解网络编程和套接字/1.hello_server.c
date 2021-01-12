//时间：2020年8月26日 14:08:33
//编写“Hello world!”服务器端

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

	int serv_sock = socket(PF_INET, SOCK_STREAM, 0);    		//IPv4协议族中面向连接的套接字，目前只有IPPROTO_TCP,所以可以填0（或者IPPROTO_TCP）
	if (serv_sock == -1)
		error_handling("socket() error");

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[1]));
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);   			//INADDR_ANY是地址为"0.0.0.0"的地址，会自动获取运行服务器端的计算机IP地址，不必亲自输入。如果同一计算机中已分配多个IP地址，则只要端口号一致就可以从不同IP地址接收数据。因此服务器端优先考虑这种方式

	if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");

	if (listen(serv_sock, 5) == -1)                				//只有调用了listen函数，客户端才能进入可发送连接请求的状态，如果客户端提前调用connect函数将发生错误
		error_handling("listen() error");          				//调用listen函数即可生成服务器端套接字-门卫  参数2backlog决定了连接请求等待队列的长度-等候室的大小   

	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size = sizeof(clnt_addr);
	int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);        	//从等待队列取出1个连接请求，创建套接字并完成连接请求（自动的），该单独创建的套接字与客户端建立连接后进行数据交换
	if (clnt_sock == -1)                                                                 		//如果等待队列为空，则accept函数不会返回，直到队列中出现新的客户端连接
		error_handling("accept() error");

	char message[] = "Hello World";
	write(clnt_sock, message, sizeof(message));
	close(clnt_sock);
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);       				//puts() 只能向标准输出流输出，而 fputs() 可以向任何流输出。
	fputc("\n", stderr);          				//使用 puts() 时，系统会在自动在其后添加换行符；而使用 fputs() 时，系统不会自动添加换行符。
	exit(1);
}