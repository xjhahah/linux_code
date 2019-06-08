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

}


//进入事件循环
void EpollServer::EventLoop()
{

}
