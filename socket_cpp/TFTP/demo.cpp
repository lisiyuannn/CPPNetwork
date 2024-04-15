//|操作码|文件名|0|模式|0
//操作码：01（读），02（写），03（数据），04（ACK），05（ERROR），06（OACK）
//模式：octet（二进制）
//|操作码|块编号|数据
//ACK:|操作码|块编号
//ERROR:|操作码|差错码|差错信息|0

//TFTP客户端代码
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

void do_download(int, sockaddr_in);

int main(int argc, char const *argv[])
{
    // if(argc != 2)
    // {
    //     perror("argc error");
    //     exit(1);
    // }


    //第一步，创建套接字
    int sockfd;

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("fail to socket");
        exit(1);
    }

    //绑定本地网络地址
    struct sockaddr_in hostaddr;
    hostaddr.sin_family = AF_INET;
    hostaddr.sin_addr.s_addr = inet_addr("42.51.43.71");
    hostaddr.sin_port = htons(8080);
    if(bind(sockfd, (sockaddr *)&hostaddr, (socklen_t)sizeof(hostaddr)))
    {
        perror("fail to bind host ipaddr");
        exit(1);
    }
    socklen_t addr_len = sizeof(sockaddr_in);

    //以下代码用来获取serverip地址
    //***********************************************************************
    struct sockaddr_in serveraddr;  //服务器网络地址结构体
    char temp[128] = "";
    temp[127] = '\0';

    if(recvfrom(sockfd, temp, 128, 0, (sockaddr *)&serveraddr, &addr_len) == -1)
    {
        perror("fail to get ipaddr");
        exit(1);
    }
    cout << temp << endl;
    serveraddr.sin_port = htons(69);
    cout << "ip: " << inet_ntoa(serveraddr.sin_addr) << endl;
    cout << "port: " << ntohs(serveraddr.sin_port) << endl;
    //***********************************************************************

    //下载
    cout << "ready to download" << endl;
    do_download(sockfd, serveraddr);
    
    return 0;
}

void do_download(int sockfd, sockaddr_in serveraddr)
{
    socklen_t addr_len = sizeof(sockaddr_in);
    //构建文件缓冲区
    // char filename[128] = "";
    // cout << "stdin filename please!!!" << endl;
    // fgets(filename, 128, stdin);
    // cout << "file name is " << filename << endl;

    //构建请求指令并发送给服务器，例如：01test.txt0octet0
    int cmd_len;
    char ftp_cmd[1024] = "";
    int flags = 0;  //文件创建标识
    unsigned short num = 0;  //文件编号，从1开始
    ssize_t bytes;
    int filefd; //打开文件后返回的文件描述符

    //构建请求指令并发送给服务器，例如：01test.txt0octet0
    //cmd_len = sprintf(ftp_cmd, "%c%c%s%c%s%c", 0, 1, filename, 0, "octet", 0);
    strcat(ftp_cmd, "0");
    strcat(ftp_cmd, "1");
    strcat(ftp_cmd, "test.txt");
    strcat(ftp_cmd, "0");
    strcat(ftp_cmd, "octet");
    strcat(ftp_cmd, "0");
    cout << "ftp_cmd: " << ftp_cmd << endl;
    cmd_len = strlen(ftp_cmd);
    //发送
    if(sendto(sockfd, ftp_cmd, cmd_len, 0, (sockaddr *)&serveraddr, (socklen_t)sizeof(serveraddr)) == -1)
    {
        perror("fail to send ftp_cmd");
        exit(1);
    }
    else
    {
        cout << "send success!" << endl;
    }
    

    //接收服务器数据并处理
    while(1)
    {
        if((bytes = recvfrom(sockfd, ftp_cmd, sizeof(ftp_cmd), 0, (sockaddr *)&serveraddr, &addr_len)) == -1)
        {
            perror("fail to recvfrom");
            exit(1);
        }

        //判断操作码
        if(ftp_cmd[1] == '5')
        {
            cout << "error:" << ftp_cmd+5 << endl;
        }
        else if(ftp_cmd[1] == '3')
        {
            if(flags == 0)
            {//第一次接收需要创建文件
                if((filefd = open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664)) < 0)
                {
                    perror("fail to open file");
                    exit(1);
                }
            }

            //文件内追加数据
            //获取数据编号
            unsigned short no = (ftp_cmd[2]-'0')*10 + (ftp_cmd[3]-'0');
            if((num+1 == no) && (bytes == 516))
            {
                num = no;
                if(write(filefd, ftp_cmd+4, bytes-4) < 0)
                {
                    perror("fail to write");
                    exit(1);
                }
                cout << "writing..." << endl;
                //当文件写入完毕后，给服务器发送ACK
                ftp_cmd[1] = '4';
                if(sendto(sockfd, ftp_cmd, 4, 0, (sockaddr *)&serveraddr, (socklen_t)sizeof(serveraddr)) == -1)
                {
                    perror("fail to send ack");
                    exit(1);
                }
                
            }
            else if((num+1 == no) && (bytes < 516))
            {
                num = no;
                if(write(filefd, ftp_cmd+4, bytes-4) < 0)
                {
                    perror("fail to write");
                    exit(1);
                }

                //当文件写入完毕后，给服务器发送ACK
                ftp_cmd[1] = '4';
                if(sendto(sockfd, ftp_cmd, 4, 0, (sockaddr *)&serveraddr, (socklen_t)sizeof(serveraddr)) == -1)
                {
                    perror("fail to send ack");
                    exit(1);
                }
                cout << "down seccuss!" << endl;
                return;
            }    
        }    
    }
}

