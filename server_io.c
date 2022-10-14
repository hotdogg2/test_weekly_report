#include<stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <string.h>
#include<errno.h>

#include "wrap.h"

#define SERV_PORT 2022
#define OPEN_MAX 5000

int main(int argc, char*argv)
{
    int lfd,cfd,sfd;
    int n,i,num = 0;
    ssize_t nready,efd,res;
    char buf[BUFSIZ],str[INET_ADDRSTRLEN];
    socklen_t clie_len;
    int opt;

    struct sockaddr_in serv_addr,clie_addr;
    struct epoll_event tep,ep[OPEN_MAX]; //所需监听事件，分别为clt，wait的参数

    lfd = Socket(AF_INET,AF_STREAM,0);

    setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));  //端口复用

    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(lfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    Listen(lfd,20);

    efd = epoll_create(OPEN_MAX); //创建epoll模型
    if(efd == -1)
        perr_exit("epoll_create error");
    tep.events = EPOLLIN;
    tep.data.fd = lfd;  //监听事件为读
    res = epoll_clt(efd,EPOLL_CLT_ADD,LFD,&tep);  //epoll操作
    if(res ==-1)
        perr_exit("epoll_clt error");
    
    while(1)
    {
        nready = epoll_wait(efd,ep,OPEN_MAX,-1);//监听事件(阻塞，直到有客户端连接)
        if (nready==-1)
            perr_exit("epoll_wait error");
        for(i=0;i<nready;i++)
        {
            if(!ep[i].events &EPOLLIN)
                continue;
            if(ep[i].lfd == lfd) 
            {
                clie_len = sizeof(clie_addr); //连接新客户端
                cfd = Accept(lfd,(struct sockaddr*)&clie_addr,*clie_len);

                print("cfd--%d,client--%d",cfd,++num);
                tep.events = EPOLLIN;
                tep.lfd = cfd;
                res = epoll_clt(efd,EPOLL_CLT_ADD,cfd,&tep); //将客户端添加入红黑树
                if(res==-1)
                    perr_exit("epoll_clt error");

            }
            else //读文件
            {
                sfd = ep[i].data.fd;
                n = Read(sfd,buf,sizeof(buf));
                if(n==0) //客户端关闭
                {
                    res = epoll_clt(efd,EPOLL_CLT_DEL,SFD,NULL);
                    if(res==-1)
                        perr_exit("epoll_clt error");
                    Close(sfd);
                }
                else if(n<0)  //出错
                {
                    res = epoll_clt(efd,EPOLL_CLT_DEL,SFD,NULL);
                    if(res==-1)
                        perr_exit("epoll_clt error");
                    Close(sfd);
                }
                else
                {
                    Write(sfd,buf,n);
                }
            }
        }
    }
    
    return 0;
}