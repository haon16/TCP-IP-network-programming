//2020年9月22日 17:42:02
//复制文件描述符

/*
sep_serv.c示例中的2个FILE指针，文件描述符及套接字之间的关系

FILE指针（读模式） <--->
                         文件描述符 <---> 套接字
FILE指针（写模式） <--->

读模式FILE指针和写模式FILE指针都是基于同一个文件描述符创建的，因此针对任意一个FILE指针调用fclose函数时都会关闭文件描述符，也就是终止套接字

销毁套接字时再也无法进行数据交换，如何进入可以输入但无法输出的半关闭状态：创建FILE指针前先复制文件描述符

FILE指针（读模式） <--->  文件描述符（原件） <--->
                            | 复制                套接字
FILE指针（写模式） <--->  文件描述符（副本） <--->

套接字和文件描述符之间的关系：销毁所有文件描述符后才能销毁套接字。
也就是说针对写模式FILE指针调用fclose函数时，只能销毁与该FILE指针相关的文件描述符，无法销毁套接字
但是此时调用fclose函数只是关闭其中1个文件描述符，并不能进入半关闭状态，另一个描述符不会发送EOF,还可以同时进行I/O

复制文件描述符：为了访问同一文件或套接字，创建另一个文件描述符
文件描述符（5） <--->
    | COPY            文件
文件描述符（7） <--->
*/

#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char str1[] = "Hi~ \n";
    char str2[] = "It's nice day~ \n";

    int cfd1 = dup(1);              //参数1：需要复制的文件描述符
    int cfd2 = dup2(cfd1, 7);       //参数2：明确指定的文件描述符整数值
    
    printf("fd1=%d, fd2=%d \n", cfd1, cfd2);    //3, 7
    write(cfd1, str1, sizeof(str1));            //验证是否进行了实际复制
    write(cfd2, str2, sizeof(str2));

    close(cfd1);
    close(cfd2);
    write(1, str1, sizeof(str1));               //终止复制的文件描述符，但仍有1个描述符，因此可以进行输出
    close(1);
    write(1, str2, sizeof(str2));               //终止最后的描述符后，无法继续输出
    
    return 0;
}