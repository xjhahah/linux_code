#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

using namespace std;

#define EPOLL_SIZE 128

int StartUp(int port)
{
  int sock = socket(AF_INET,SOCK_STREAM,0);
  if(sock < 0)
  {
    
    cerr << "socket error" <<endl;
    exit(1);
  }

  struct sockaddr_in local;
  local.sin_family = AF_INET;
  local.sin_port = htons(port);
  local.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(sock,(struct sockaddr*)&local, sizeof(local) < 0))
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

//事件处理函数
void HandlerEvents(int epfd,struct epoll_revs[],int num,int listen_sock)
{

}
int main()
{
  //创建 socket
  int listen_sock = StartUp(6666);

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
        cout << "epoll wait default..." << endl;
        break;
      default:
        //调用成功，处理事件
        HandlerEvents(epfd,revs,num,listen_sock);
        break;
    }
  }
  return 0;
}
