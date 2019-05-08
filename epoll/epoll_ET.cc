#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include <fcntl.h>

using namespace std;

#define EPOLL_SIZE 128


void SetNonBlock(int sock)
{
  int fl = fcntl(sock,F_GETFL);

}

int StartUp(int port)
{
  int sock = socket(AF_INET,SOCK_STREAM,0);
  if(sock < 0)
  {
    
    cerr << "socket error" <<endl;
    exit(1);
  }

  int opt = 1;
  setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

  struct sockaddr_in local;
  local.sin_family = AF_INET;
  local.sin_port = htons(port);
  local.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(sock,(struct sockaddr*)&local, sizeof(local)) < 0)
  {
    cerr << "bind error" <<endl;
    exit(2);
  }

  if(listen(sock,10) < 0)
  {
    cerr << "listen error" <<endl;
    exit(3);
  }

  return sock;
}

void AddEventsToEpoll(int epfd,int sock,uint32_t events) 
{
  struct epoll_event ev;
  ev.data.fd = sock;
  ev.events = events;

  //添加任务
  epoll_ctl(epfd,EPOLL_CTL_ADD,sock,&ev);
  cout << "epoll add fd: " << sock << endl;
}

//修改事件
void ExchangeEventFromEpoll(int epfd,int sock,uint32_t events)
{
  struct epoll_event ev;
  ev.events = events;
  ev.data.fd = sock;

  epoll_ctl(epfd,EPOLL_CTL_MOD,sock,&ev);
  cout << "mod epoll fd : " << sock << endl;
}

//将事件从epoll中删除
void DelEventFromEpoll(int epfd,int sock)
{
  cout << "delete epoll fd: " << sock << endl;
  epoll_ctl(epfd,EPOLL_CTL_DEL,sock,NULL);
}
//事件处理函数
void HandlerEvents(int epfd,struct epoll_event revs[],int num,int listen_sock)
{
  for(int i=0; i < num; ++i)
  {
    int sock = revs[i].data.fd;
    uint32_t events = revs[i].events;

    if(events & EPOLLIN)  //有事件加入
    {
      if(listen_sock == sock) //说明是套接字
      {
        struct sockaddr_in peer; 
        socklen_t len = sizeof(peer);

        int new_sock = accept(sock,(struct sockaddr*)&peer,&len);
        if(new_sock < 0)
        {
          cerr << "accept error" << endl;
          continue;
        }
        //有链接接入
        AddEventsToEpoll(epfd,new_sock,EPOLLIN);
        cout << "get a new link..." << endl;
      }
      else  //其他文件描述符
      {
        //read
        char buf[10240];
        ssize_t s = recv(sock,buf,sizeof(buf)-1,0);
        if(s > 0)
        {
          buf[s] = 0;
          cout << "########################" << endl;
          cout << buf << endl;
          cout << "########################" << endl;
          ExchangeEventFromEpoll(epfd,sock,EPOLLOUT);
        }
        else if(s==0)
        {
          close(sock);
          DelEventFromEpoll(epfd,sock);
          cout << "client id quit..." << endl;
        }
        else 
        {
          close(sock);
          DelEventFromEpoll(epfd,sock);
        }
      }
    }
    else if(events & EPOLLOUT)  //写事件就绪
    {
      string response = "HTTP/1.1 200 OK\r\n\r\n<html><h2>Hello, epoll server</h2></html>";
      send(sock,response.c_str(),response.size(),0);
      close(sock);
      DelEventFromEpoll(epfd,sock);
    }
    else{
      //TODO
    }
  }
}
int main()
{
  //创建 socket
  int listen_sock = StartUp(8080);

  //创建epoll模型  就绪队列 红黑树 回调函数
  int epfd = epoll_create(256);

  if(epfd < 0)
  {
    cerr << "epoll_create error" << endl;
    exit(4);
  }

  cout << "epoll fd: " << epfd << endl;
  //添加事件 
  AddEventsToEpoll(epfd, listen_sock,EPOLLIN); //EPOLLIN 表示对应的文件描述符可以读

  struct epoll_event revs[EPOLL_SIZE];
  while(1)
  {
    int timeout = 500;
    int num = epoll_wait(epfd,revs,EPOLL_SIZE,timeout);  //收集epoll监控就绪的的事件
    switch(num)
    {
      case 0:
        cout << "time out..." << endl;
        break;
      case -1:
        cerr << "epoll wait default..." << endl;
        break;
      default:
        //调用成功，处理事件
        HandlerEvents(epfd,revs,num,listen_sock);
        break;
    }
  }
  close(epfd);
  return 0;
}
