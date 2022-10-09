#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include <arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include"wrap.h"

#define SERV_PORT 2021
#define SERV_IP "192.168.232.128"

int main(void)
{
    int lfd,cfd;
    struct sockaddr_in serv_addr,clie_addr;
    socklen_t clie_len;
    char buf[BUFSIZ],clie_IP[BUFSIZ];

    lfd = Socket(AF_INET,SOCK_STREAM,0);

    //初始化socket
    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //绑定服务端IP和端口号
    Bind(lfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    //允许访问上限
    Listen(lfd,128);

    cfd = Accept(lfd,(struct sockaddr*)&clie_addr,&clie_len);
    printf("the client is %s,%d\n",inet_ntop(AF_INET,&clie_addr.sin_addr.s_addr,clie_IP,sizeof(clie_IP)),ntohs(clie_addr.sin_port));

    Read(cfd,buf,sizeof(buf));
    Write(cfd,buf,sizeof(buf));
    Close(lfd);
    Close(cfd);

    return 0;
}
