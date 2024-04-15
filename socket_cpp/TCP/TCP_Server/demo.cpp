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

    //绑定服务器网络地址
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("42.51.43.71");
    serveraddr.sin_port = htons(8080);

    if(bind(sockfd, (sockaddr *)&serveraddr, (socklen_t)sizeof(sockaddr_in)) == -1)
    {
        perror("fail to bind");
        exit(1);
    }
    else
    {
        cout << "bind success!" << endl;
    }

    //监听 listen
    if(listen(sockfd, 10) == -1)
    {
        perror("fail to listen");
        exit(1);
    }
    else
    {
        cout << "listening..." << endl;
    }

    //接收连接请求 accept
    //构建客户端网络地址结构体
    struct sockaddr_in clientaddr;
    socklen_t addrlen = (socklen_t)sizeof(sockaddr_in);
    int acceptfd;
    char buff[N] = "";
    while(1)
    {
        if((acceptfd = accept(sockfd, (sockaddr *)&clientaddr, &addrlen)) == -1)
        {
            perror("fail to accept");
            exit(1);
        }
        else
        {
            cout << "got you!" << endl;
        }
        //收到请求后创建新进程，然后继续监听
        if(fork() > 0)
        {//父进程执行代码
            
        }
        else
        {//子进程执行这段代码
            //先接受再发送数据
            while(1)
            {
                //cout << "acceptfd: " << acceptfd << endl;
                if(recv(acceptfd, buff, N, 0) == -1)
                {
                    perror("fail to recv");
                    exit(1);
                }

                cout << ntohs(clientaddr.sin_port) << ":";
                cout << buff << endl;

                if(!strcmp(buff, "quit"))
                {
                    close(acceptfd);
                    cout << ntohs(clientaddr.sin_port) << ": closed" << endl;
                    exit(0);
                }
                
                strcat(buff, "^_^");

                if(send(acceptfd, buff, N, 0) == -1)
                {
                    perror("fail to send");
                    exit(1);
                }
            }

            
        }
    }

    close(sockfd);

    
    return 0;
}
