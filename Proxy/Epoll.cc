#include "Epoll.h"

//启动epoll服务器
void EpollServer::Start()
{
  _listenfd = socket(AF_INET,SOCK_STREAM,0);
  if(_listenfd==-1){
    ErrorDebug("socket error"); //出错输出到日志当中
    return;
  }
  struct sockaddr_in addr;
  memset(&addr,0,sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port=htons(_port);

  if(bind(_listenfd,(struct sockaddr*)&addr,sizeof(addr)) < 0){
    ErrorDebug("bind error...");
    return;
  }

  TraceDebug("server is running on port: %d",_port);

  //创建epoll
  _event_fd = epoll_create(_MAX_EVENT);
  if(_event_fd < 0){
    ErrorDebug("epoll_create false...");
    return;
  }
  //如果监听成功，进入epoll事件循环
  EventLoop();
}


//进入事件循环
void EpollServer::EventLoop()
{
  //注册事件函数
  SetNonblocking(_listenfd);
  OpEvent(_listenfd,EPOLLIN,EPOLL_CTL_ADD,__LINE__);

  //事件列表
  struct epoll_event events[_MAX_EVENT];

  while(1){
    //获取就绪事件
    int nfds = epoll_wait(_event_fd,events,_MAX_EVENT,-1);  //成功返回文件描述符数目，失败返回-1

    if(nfds < 0){
      ErrorDebug("epoll_wait error...");
      break;
    }

    if(nfds == 0){
      ErrorDebug("epoll_wait waiting timeout...");
      break;
    }

    for(int i = 0;i < nfds; ++i){
      if(events[i].data.fd == _listenfd){

      }
    }
  }
}
