//时间：2019年8月13日20:23:23
//完成端口

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <mswsock.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#define MAX_COUNT 1024
#define MAX_RECV_COUNT 1500

SOCKET g_allsockets[MAX_COUNT];
OVERLAPPED g_allOlp[MAX_COUNT];
int g_count;
//int g_newsocketID;
HANDLE hPort;
HANDLE *pThread;
int nProcessorsCount;


//接收缓冲区
char g_strrecv[1500] = {0};

int PostAccept(void);    //加void表示不接受参数， 如果为空，表示参数个数不确定
int PostRecv(int index);
int PostSend(int index);
void Clear(void)
{
	for (int i = 0; i < g_count; i++)
	{
		if (0 == g_allsockets[i])
			continue;
		closesocket(g_allsockets[i]);
		WSACloseEvent(g_allOlp[i].hEvent);
	}
}

BOOL g_flag = TRUE;

//线程函数
DWORD WINAPI ThreadProc(LPVOID lpPrameter);

//回调函数  (点控制台退出)
BOOL WINAPI fun(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		CloseHandle(hPort);
		Clear();

		g_flag = FALSE;

		//释放线程句柄
		for (int i = 0; i < nProcessorsCount; i++)
		{
			//TerminateThread();
			CloseHandle(pThread[i]);
		}
		free(pThread);

		break;
	}

	return TRUE;
}

int main()
{
	//控制台退出 主函数投递一个监视
	SetConsoleCtrlHandler(fun, TRUE);

	WSADATA wsadata;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		printf("网络库打开失败\n");
		return -1;
	}

	if (2 != HIBYTE(wsadata.wVersion) || 2 != LOBYTE(wsadata.wVersion))
	{
		printf("版本错误\n");
		WSACleanup();
		return -1;
	}

	SOCKET socketServer = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == socketServer)
	{
		printf("创建服务器socket失败，错误码:%d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	struct sockaddr_in socketServerMsg;
	socketServerMsg.sin_family = AF_INET;
	socketServerMsg.sin_port = htons(12345);
	socketServerMsg.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(socketServer, (struct sockaddr *)&socketServerMsg, sizeof(socketServerMsg)))
	{
		printf("绑定IP和端口失败，错误码：%d\n", WSAGetLastError());
		closesocket(socketServer);
		WSACleanup();
		return -1;
	}

	
/*
功能：1.创建一个完成端口    2.将完成端口与SOCKET绑定再一起
参数1：1.创建完成端口    INVALID_HANDLE_VALUE  (1)此时参数2为NULL
											  (2)参数3忽略  填个0
	  2.绑定的话   服务器socket
参数2：1.创建完成端口    NULL
	  2.绑定的话    完成端口变量
参数3：1.创建完成端口   0
	  2.绑定的话    （1）再次传递socketServer    也可以传递一个下标，做编号
					（2）与系统接收到的对应的数据关联在一起
参数4：1.创建完成端口   （1）允许此端口上最多同时运行的线程数量
					   （2）填CPU的核数即可    咱们自己获取    GetSystemInfo
					   (3)可填写0   表示默认是CPU核数
	  2.绑定的话     （1）忽略此参数，填0就行了
					（2）当说忽略的时候，我们填啥都是没有作用的，我们一般填0
返回值：1.成功  （1）当参数2为NULL,   返回一个新的完成端口
				（2）当参数2不为NULL    返回自己
				（3）当参数1为socket    返回与socket绑定后的端口
	   2.失败返回0    (1)GetLastError()
					 (2)完成端口也是windows的一种机制      不是网络专属，文件操作均可以
*/
	//创建完成端口
	hPort =  CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (0 == hPort)
	{
		int a = GetLastError();
		printf("%d\n", a);
		closesocket(socketServer);
		WSACleanup();
		return -1;
	}

	g_allsockets[g_count] = socketServer;
	g_allOlp[g_count].hEvent = WSACreateEvent();
	g_count++;

	//绑定
	HANDLE hPort1 = CreateIoCompletionPort((HANDLE)socketServer, hPort, 0, 0);
	if (hPort1 != hPort)
	{
		int a = GetLastError();
		printf("%d\n", a);
		CloseHandle(hPort);
		closesocket(socketServer);
		WSACleanup();
		return -1;
	}

	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))
	{
		printf("监听失败，错误码：%d\n", WSAGetLastError());
		CloseHandle(hPort);
		closesocket(socketServer);
		WSACleanup();
		return -1;
	}


	if (0 != PostAccept())
	{
		Clear();
		WSACleanup();
		return -1;
	}

	//创建线程数量有了
	SYSTEM_INFO SystemProcessorsCount;
	GetSystemInfo(&SystemProcessorsCount);
	nProcessorsCount = SystemProcessorsCount.dwNumberOfProcessors;
	//创建
/*
	功能：创建一根线程
	参数1：1.线程句柄是否被继承    NULL不继承
		  2.指定线程的执行权限     NULL默认的
	参数2：1.线程栈大小      填0，系统使用默认大小    1M
		  2.字节为单位
	参数3：线程函数地址     （1）DWORD WINAPI ThreadProc(LPVOID lpPrameter)
							(2)参数LPVOID lpPrameter，就是参数4传递进来的数据
	参数4：外部给线程传递数据
	参数5：1. 0     线程立即执行
		  2.  CREATE_SUSPENDED    线程创建完挂起状态，调用ResumeThread启动函数
		  3.  STACK_SIZE_PARAM_IS_A_RESERVATION    (1)设置了，参数2就是栈保留大小    虚拟内存上栈的大小     1M
												   （2）未设置，就是栈提交大小    物理内存上栈的大小     4KB
	参数6：线程ID    可以填NULL
	返回值：1.成功   返回线程句柄    （1）内核对象
									（2）最后要释放   CloseHandle
		   2.失败    NULL    GetLastError   得到错误码
*/
	pThread = (HANDLE)malloc(sizeof(HANDLE) * nProcessorsCount);
	for (int i = 0; i < nProcessorsCount; i++)
	{
		pThread[i] = CreateThread(NULL, 0, ThreadProc, hPort, 0, NULL);
		if (NULL == pThread[i])
		{
			int a = GetLastError();
			printf("%d\n", a);
			CloseHandle(hPort);
			closesocket(socketServer);
			WSACleanup();
			return -1;
		}
	}

	//阻塞
	while (g_flag)
	{
		Sleep(1000);    //主线程挂起1s，腾出1S给CPU,处理其他线程，对CPU友好
	}

	//释放线程句柄
	for (int i = 0; i < nProcessorsCount; i++)
	{
		CloseHandle(pThread[i]);
	}
	free(pThread);

	CloseHandle(hPort);
	Clear();
	WSACleanup();
	system("pause");
	return 0;
}

//线程函数
DWORD WINAPI ThreadProc(LPVOID lpPrameter)
{
/*
无通知时，线程挂起，不占用CPU的时间，非常棒
功能：尝试从指定的I/O完成端口取I/O完成数据包
参数1：完成端口    咱们要从主函数传进来
参数2：接收或者发送的字节数
参数3：完成端口函数参数3传进来的
参数4：重叠结构
参数5：1.等待时间    当没有客户端响应时候，通知队列里什么都没有，咱们这里也get不到什么东西，那么等一会还是一直等
	  2.INFINITE    一直等，闲着也是闲着
返回值：1.成功返回TRUE
       2.失败返回FALSE   GetLastError
	                    continue
*/
	HANDLE port = (HANDLE)lpPrameter;
	DWORD NumberOfBytes;
	ULONG_PTR index;
	LPOVERLAPPED lpOverlapped;

	while (1)
	{
		BOOL bFlag = GetQueuedCompletionStatus(port, &NumberOfBytes, &index, &lpOverlapped, INFINITE);
		if (FALSE == bFlag)
		{
			int a = GetLastError();
			if (64 == a)
			{
				printf("force close\n");
			}
			printf("错误码：%d\n", a);
			continue;
		}

		//处理
		//accept
		if (0 == index)
		{
			printf("accept\n");
			//绑定到完成端口
			HANDLE hPort1 = CreateIoCompletionPort((HANDLE)g_allsockets[g_count], hPort, g_count, 0);
			if (hPort1 != hPort)
			{
				int a = GetLastError();
				printf("%d\n", a);
				closesocket(g_allsockets[g_count]);
				continue;
			}
			PostSend(g_count);
			//新客户端投递recv
			PostRecv(g_count);
			g_count++;
			PostAccept();
		}
		else
		{
			if (0 == NumberOfBytes)
			{
				//客户端下线
				printf("close\n");
				//关闭
				closesocket(g_allsockets[index]);
				WSACloseEvent(g_allOlp[index].hEvent);

				g_allsockets[index] = 0;
				g_allOlp[index].hEvent = NULL;
			}
			else
			{
				if (g_strrecv[0] != 0)
				{
					//收到recv
					printf("%s\n", g_strrecv);
					memset(g_strrecv, 0, sizeof(g_strrecv));
					//投递
					PostRecv(index);

				}
				else
				{
					//send
					printf("send ok\n");
				}
			}
		}
	}
	return 0;
}

int PostAccept()
{
	g_allsockets[g_count] = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_allOlp[g_count].hEvent = WSACreateEvent();
	//g_newsocketID = g_count;     //PostRecv和PostSend应该是传递最新一个socket，而不是socket有效个数,因为删了socket对导致有效个数g_count变化，此时再传递g_count会出错       不行还是有问题

	char str[1500] = {0};
	DWORD recv_count;
	BOOL bFlag = AcceptEx(g_allsockets[0], g_allsockets[g_count], str, 0, sizeof(struct sockaddr_in) + 16, sizeof(struct sockaddr_in) + 16, &recv_count, &g_allOlp[0]);
	
	//if (TRUE == bFlag)
	//{
	//	HANDLE hPort1 = CreateIoCompletionPort((HANDLE)g_allsockets[g_count], hPort, g_count, 0);
	//	if (hPort1 != hPort)
	//	{
	//		int a = GetLastError();
	//		printf("%d\n", a);
	//		closesocket(g_allsockets[g_count]);
	//		//continue;
	//	}

	//	printf("accept");
	//	PostRecv(g_count);
	//	PostSend(g_count);
	//	g_count++;
	//	PostAccept();
	//	return 0;
	//}
	//else
	//{
	//	int a = WSAGetLastError();
	//	if (ERROR_IO_PENDING == a)
	//	{
	//		return 0;
	//	}
	//	else
	//	{
	//		return a;
	//	}
	//}
	int a = WSAGetLastError();
	if (ERROR_IO_PENDING != a)
	{
		//函数执行出错
		return -1;
	}
	return 0;
}

int PostRecv(int index)
{
	WSABUF wsabuf;
	wsabuf.buf = g_strrecv;
	wsabuf.len = MAX_RECV_COUNT;

	DWORD read_count;
	DWORD flag = 0;
	int nRes = WSARecv(g_allsockets[index], &wsabuf, 1, &read_count, &flag, &g_allOlp[index], NULL);
	/*if (0 == nRes)
	{
		printf("%s\n", g_strrecv);
		memset(g_strrecv, 0, MAX_RECV_COUNT);
		PostRecv(index);
		return 0;
	}
	else
	{
		int a = WSAGetLastError();
		if (ERROR_IO_PENDING == a)
		{
			return 0;
		}
		else
		{
			return a;
		}
	}*/
	int a = WSAGetLastError(); 
	if (ERROR_IO_PENDING != a)    //有问题：WSASend函数执行立即完成nRes=0, 此时取错误码a=0， a != ERROR_IO_PENDING也会进入if内部？
	{
		//函数执行出错
		return -1;
	}
	return 0;
}

int PostSend(int index)
{
	WSABUF wsabuf;
	wsabuf.buf = "链接成功";
	wsabuf.len = MAX_RECV_COUNT;

	DWORD send_count;
	DWORD flag = 0;
	int nRes = WSASend(g_allsockets[index], &wsabuf, 1, &send_count, flag, &g_allOlp[index], NULL);

	//完成端口不需要区分立即完成还是异步完成，只要完成就会产生通知，通知就会装进通知队列，线程就能取出通知
	/*if (0 == nRes)
	{
		printf("send成功\n");
		return 0;
	}
	else
	{
		int a = WSAGetLastError();
		if (ERROR_IO_PENDING == a)
		{
			return 0;
		}
		else
		{
			return a;
		}
	}*/
	int a = WSAGetLastError();       //有问题：WSASend函数执行立即完成nRes=0, 此时取错误码a=0， a != ERROR_IO_PENDING也会进入if内部？
	if (ERROR_IO_PENDING != a)
	{
		//延迟处理
		//函数执行出错
		return -1;
	}
	return 0;
}