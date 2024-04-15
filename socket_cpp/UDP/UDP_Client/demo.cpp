//创建套接字
//int socket(int family, int type, int protocol)
//family(AF_INET, AFINET6, PF_PACKET)  协议族
//type(SOCK_STREAM, SOCK_DGRAM, SOCK_RAW)  套接字类
//protocol(0, IPPROTO_TCP, IPPROTO_UDP)  协议类别

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
using namespace std;

#define N 128

int main(int argc, char const *argv[])
{
    if(argc != 3)
    {
        perror("ERROR!!!");
    }

    //使用socket函数创建套接字
    //创建一个用于UDP网络编程的套接字
    //创建成功返回文件描述符，失败返回-1
    int sockfd;  //文件描述符，从3开始
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("fail to socket");
        exit(1);
    }

    cout << sockfd << endl;

    //第二步，填充服务器网络地址结构体
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;  //协议簇  IPV4
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);  //sin_addr为IP地址结构体
    serveraddr.sin_port = htons(atoi(argv[2]));  //端口号设置，注意大小端存储

    socklen_t addrlen = sizeof(serveraddr);

    //第三步，发送数据
    //sendto函数 参数：文件描述符，发送数据缓冲区地址，缓冲区大小，flags，网络地址结构体(sockaddr *)，地址长度

    char buff_send[N] = "";  //发送数据缓冲区
    char buff_recv[N] = "";  //接收数据缓冲区

    while(1)
    {
        fgets(buff_send, N, stdin);
        buff_send[strlen(buff_send)-1] = '\0';
        if(sendto(sockfd, buff_send, N, 0, (sockaddr *)&serveraddr, addrlen) == -1)
        {
            perror("fail to sendto");
        }

        if(recv(sockfd, buff_recv, N, 0) == -1)
        {
            perror("fail to recv");
            exit(1);
        }

        //打印接收到的数据
        cout << buff_recv << endl;
    }

    //关闭套接字
    close(sockfd);

    return 0;


}
