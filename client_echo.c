#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include <arpa/inet.h>
#include<sys/socket.h>
#include"wrap.h"

#define SERV_PORT 2021
#define SERV_IP "192.168.232.128"

int main()
{
    int cfd;
    struct sockaddr_in serv_addr;
    //socklen_t serv_len;
    char buf[BUFSIZ];
    int n;

    cfd = Socket(AF_INET,SOCK_STREAM,0);

    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET ;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,SERV_IP,&serv_addr.sin_addr.s_addr);
    //连接服务器
    Connect(cfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    fgets(buf,sizeof(buf),stdin);
    Write(cfd,buf,strlen(buf));
    n = Read(cfd,buf,sizeof(buf));
    Write(STDOUT_FILENO,buf,n);

    close(cfd);
    return 0;
}