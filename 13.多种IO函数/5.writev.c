//2020年9月15日 18:07:57
//使用readv & writev函数

/*
readv & writev功能
对数据进行整合传输及发送的函数，也就是说通过writev函数可以将分散保存在多个缓冲中的数据一并发送，
通过readv函数可以由多个缓冲分别接受。因此适当使用这2个函数可以减少I/O函数的调用次数

ssize_t writev(int filedes, const struct iovec *iov, int iovcnt);
filedes：表示数据传输对象的套接字文件描述符
iov：iovec结构体数组的地址值，结构体iovec中包含待发送数据的位置和大小信息
iovcnt：向第二个参数传递的数组长度，iovec结构体的数量

struct iovec
{
    void * iov_base;  //缓冲地址
    size_t iov_len;   //缓冲大小
}
*/

//writev函数的使用方法

#include <stdio.h>
#include <sys/uio.h>

int main(int argc, char *argv[])
{
    struct iovec vec[2];
    char buf1[] = "ABCDEFG";
    char buf2[] = "1234567";

    vec[0].iov_base = buf1;
    vec[0].iov_len = 3;
    vec[1].iov_base = buf2;
    vec[1].iov_len = 4;

    int str_len = writev(1, vec, 2);    //向控制台输出数据
    puts("");
    printf("Write bytes: %d \n", str_len);

    return 0;
}