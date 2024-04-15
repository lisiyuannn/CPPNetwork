//bind函数，将套接字与ip地址结构体绑定
//int bind(描述符，ip地址结构体(sockaddr *)，ip结构体大小)

//recvfrom函数，接收对方数据和网络地址信息
//ssize_t recvfrom(描述符，接收数据缓冲区，缓冲区大小，flags，对方网络地址结构体指针，结构体长度)


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

    //第三步，将服务器网络地址结构体与套接字绑定
    if(bind(sockfd, (sockaddr *)&serveraddr, addrlen) == -1)
    {
        perror("fail to bind");
    }



    //第四步，接收数据并打印
    char buff_recv[N] = "";  //接收数据缓冲区
    buff_recv[strlen(buff_recv)-1] = '\0';
    char buff_send[N] = "";  //发送数据缓冲区
    buff_send[strlen(buff_send)-1] = '\0';
    while(1)
    {
        //准备结构体保存对方网络地址信息
        struct sockaddr_in clientaddr;
        
        if(recvfrom(sockfd, buff_recv, N, 0, (sockaddr *)&clientaddr, &addrlen) == -1)
        {
            perror("fail to recv");
            exit(1);
        }

        //打印数据，和客户端网络地址信息
        cout << "ip: " << inet_ntoa(clientaddr.sin_addr) << endl;
        cout << "port: " << ntohs(clientaddr.sin_port) << endl;
        cout << buff_recv << endl;

        //第五步，发送数据
        //sendto函数 参数：文件描述符，发送数据缓冲区地址，缓冲区大小，flags，网络地址结构体(sockaddr *)，地址长度
        strcpy(buff_send, buff_recv);
        strcat(buff_send, "^_^");

        if(sendto(sockfd, buff_send, N, 0, (sockaddr *)&clientaddr, addrlen) == -1)
        {
            perror("fail to sendto");
        }
        
    }

    //关闭套接字
    close(sockfd);

    return 0;


}
