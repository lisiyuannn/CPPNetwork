//SOCK_STREAM  tcp协议

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
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

#define N 128

int main(int argc, char const *argv[])
{
    //创建tcp套接字
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("fail to socket");
        exit(1);
    }
    cout << "sockfd: " << sockfd << endl;

    //第二步，发送客户端链接请求
    //建立服务器网络结构体
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("42.51.43.71");
    serveraddr.sin_port = htons(8080);
    
    if(connect(sockfd, (sockaddr *)&serveraddr, (socklen_t)sizeof(sockaddr_in)) == -1)
    {
        perror("fail to connect");
        exit(1);
    }
    else
    {
        cout << "connect success!" << endl;
    }

    //第三步，先发送数据，再接收数据
    char buff[N] = "";
    while(1)
    {
        fgets(buff, N, stdin);  //这里buff以‘\n’结束
        buff[strlen(buff)-1] = '\0';

        if(send(sockfd, buff, N, 0) == -1)
        {
            perror("fail to send");
            exit(1);
        }

        if(!strcmp(buff, "quit"))
        {
            close(sockfd);
            exit(0);
        }

        if(recv(sockfd, buff, N, 0) == -1)
        {
            perror("fail to recv");
            exit(1);
        }
        cout << "buff: "<< buff << endl;
    }

    return 0;
}