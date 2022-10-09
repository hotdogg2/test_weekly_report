#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include<fcntl.h>
#include<pthread.h>
#include"wrap.h"

#define SEVR_PORT 2022
//客户端地址结构和cfd
struct Msg_sock
{
    struct sockaddr_in clie_addr;
    int cfd;
};

void* my_echo(void*arg)
{
    int n;
    struct Msg_sock* ms = (struct Msg_sock*)arg;
    char buf[BUFSIZ];

    while(1)
    {
        n=Read(ms->cfd,buf,sizeof(buf));
       /* if(n==0);
        {
            printf("the client %d close \n",ms->cfd);
            break;
        }*/
        Write(ms->cfd,sizeof(buf));
    }
    Close(ms->cfd);
    return (void*)0;
}
int main(void)
{
    int lfd,cfd;
    struct sockaddr_in serv_addr,clie_addr;
    socklen_t clie_len;
    pthread_t tid;
    struct Msg_sock ms[256];
    int i = 0,err;

    lfd = Socket(AF_INET,SOCK_STREAM,0);

    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SEVR_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //绑定socket
    Bind(lfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    Listen(lfd,128);//上限

    while (1)
    {
        clie_len = sizeof(clie_addr);
        cfd = Accept(lfd,(struct sockaddr*)&clie_addr,&clie_len);//监听客户端请求
        ms[i].cfd = cfd;
        ms[i].clie_addr = clie_addr;

        err = pthread_create(&tid,NULL,my_echo,(void*)&ms[i]);//创建新线程
        if(err!=0)
            perr_exit("can't creat thread");
        pthread_detach(tid);//释放所占资源
        i++;
    }
    return 0;
}