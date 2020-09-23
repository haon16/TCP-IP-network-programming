//2020年9月17日 11:19:23
//标准I/O函数复制文件

#include <stdio.h>
#include <time.h>

#define BUF_SIZE 3  //用最短数组长度构成

int main(int argc, char* argv[])
{
    FILE *fp1 = fopen("news.txt", "r");
    FILE *fp2 = fopen("cpy.txt", "w");
    char buf[BUF_SIZE];

    clock_t cbegin = clock();
    printf("cbegin : %d\n", cbegin);

    while (fgets(buf, BUF_SIZE, fp1) != NULL)
        fputs(buf, fp2);

    clock_t cend = clock();
    printf("cend : %d\n", cend);
    printf("diffsec : %d\n", (cend-cbegin)/CLOCKS_PER_SEC);

    fclose(fp1);
    fclose(fp2);
    return 0;
}

/*
标准I/O函数的缺点: 1.不容易进行双向通信  2.有时可能频繁调用fflush函数  3.需要以FILE结构体指针的形式返回文件描述符

打开文件时，如果希望同时进行读写操作，则应以r+,w+,a+模式打开。但因为缓冲的缘故，每次切换读写工作状态时应调用fflush函数。
这也会影响基于缓冲的性能提升。而且为了使用标准I/O函数，需要FILE结构体指针。而创建套接字时默认返回文件描述符，因此需要将
文件描述符转化为FILE指针
*/