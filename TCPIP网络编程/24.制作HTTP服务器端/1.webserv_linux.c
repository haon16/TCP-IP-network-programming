//2020年10月7日 10:54:29
//实现基于Linux的多线程Web服务器端

/*
HTTP请求消息结构：
    请求行      GET /index.html  HTTP/1.1                  //请求index.html文件，希望以1.1版本的HTTP协议进行通信
    消息头      User-Agent : Mozilla/5.0
                Accept:image/gif, image/jpeg
                . . . .
    空行
    消息体      消息体仅在以POST方式请求时插入

响应消息的结构：
    状态行      HTTP/1.1 200 OK                            //含有关于客户端请求的处理结果：我想用HTTP1.1版本进行响应，你的请求已正确处理（200 OK）   （200 OK:成功处理了请求； 404 Not Found:请求的文件不存在； 400 Bad Request:请求方式错误，请检查） 
    消息头      Server : SimpleWebServer                   //消息头中含有传输的数据类型和长度信息：服务器端名为SimpleWebServer，传输的数据类型为text/html（html格式的文本数据）。数据长度不超过2048字节
                Content-type : text/html            
                Content-length : 2048
                . . . .
    空行
    消息体      <html>
                . . . .
                <body>
                . . . .
                </html>
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define SMALL_BUF 100

void* request_handler(void* arg);
void send_data(FILE* fp, char* ct, char* file_name);
char* content_type(char* file);
void send_error(FILE* fp);
void error_handling(char* message);

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
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if (listen(serv_sock, 20) == -1)
        error_handling("listen() error");

    int clnt_sock;
    struct sockaddr_in clnt_adr;
    int clnt_adr_size;
    pthread_t t_id;
    while (1)
    {
        clnt_adr_size = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
        printf("Connection Request : %s:%d\n", inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));

        pthread_create(&t_id, NULL, request_handler, &clnt_sock);
        pthread_detach(t_id);
    }

    close(serv_sock);
    return 0;
}

void* request_handler(void *arg)
{
    int clnt_sock = *((int *)arg);
    char req_line[SMALL_BUF];
    char method[10];
    char ct[15];
    char file_name[30];

    FILE* clnt_read = fdopen(clnt_sock, "r");
    FILE* clnt_write = fdopen(dup(clnt_sock), "w");
    fgets(req_line, SMALL_BUF, clnt_read);
    if (strstr(req_line, "HTTP/") == NULL)          //查看是否为HTTP提出的请求
    {
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return;
    }

    strcpy(method, strtok(req_line, " /"));
    strcpy(file_name, strtok(NULL, " /"));          //查看请求文件名
    strcpy(ct, content_type(file_name));            //查看Content-type
    if (strcmp(method, "GET") != 0)                 //查看是否为GET方式的请求
    {
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return;
    }
    fclose(clnt_read);
    send_data(clnt_write, ct, file_name);           //响应
}

void send_data(FILE* fp, char* ct, char* file_name)
{
    char protocol[] = "HTTP/1.0 200 OK\r\n";
    char server[] = "Server:Linux Web Server\r\n";
    char cnt_len[] = "Content-length:2048\r\n";
    char cnt_type[SMALL_BUF];
    char buf[BUF_SIZE];

    sprintf(cnt_type, "Content-type:%s\r\n\r\n", ct);
    FILE* send_file = fopen(file_name, "r");
    if (send_file == NULL)
    {
        send_error(fp);
        return;
    }

    //传输头信息
    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_len, fp);
    fputs(cnt_type, fp);
    
    //传输请求数据
    while (fgets(buf, BUF_SIZE, send_file) != NULL)
    {
        fputs(buf, fp);
        fflush(fp);
    }

    fflush(fp);
    fclose(fp);
}

char* content_type(char* file)
{
    char extension[SMALL_BUF];
    char file_name[SMALL_BUF];
    strcpy(file_name, file);
    strtok(file_name, ".");                 //strtok：分解字符串，分隔符为“.”，该函数返回被分解的第一个子字符串，未找到是返回字符串首地址，如果没有可检索的字符串，则返回一个空指针。
    strcpy(extension, strtok(NULL, "."));

    if (!strcmp(extension, "html") || !strcmp(extension, "htm"))
        return "text/html";
    else
        return "text/plain";
}

void send_error(FILE* fp)               //发生错误时传递消息
{
    char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
    char server[] = "Server:Linux Web Server\r\n";
    char cnt_len[] = "Content-length:2048\r\n";
    char cnt_type[] = "Content-type:text/html\r\n\r\n";
    char content[] = "<html><head><title>NETWORK</title></head><body><font size=+5><br>发生错误！查看请求文件名和请求方式！</font></body></html>";

    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_len, fp);
    fputs(cnt_type, fp);
    fflush(fp);
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}