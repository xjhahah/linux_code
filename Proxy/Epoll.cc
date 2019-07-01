#include "Epoll.h"

const size_t  EpollServer::_MAX_EVENT = 10000;
//启动epoll服务器
void EpollServer::Start()
{
  //创建套接字
  _listenfd = socket(AF_INET,SOCK_STREAM,0);
  if(_listenfd==-1){
    ErrorDebug("socket error"); //出错输出到日志当中
    return;
  }

  //绑定端口
  struct sockaddr_in addr;
  memset(&addr,0,sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port=htons(_port);

  if(bind(_listenfd,(struct sockaddr*)&addr,sizeof(addr)) < 0){
    ErrorDebug("bind error...");
    return;
  }

  //监听套接字
  if(listen(_listenfd,1000) < 0){
    ErrorDebug("listen error...");
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
        
        //如果是套接字事件就绪
        //进入accept操作
        struct sockaddr_in client;
        socklen_t len = sizeof(client);

        //接受请求连接 
        int newfd = accept(_listenfd,(struct sockaddr*)&client,&len);
        if(newfd < 0){
          ErrorDebug("accept error...");
          continue;
        }

        TraceDebug("accept new connect:%s:%d",inet_ntoa(client.sin_addr),ntohs(client.sin_port));

        //新连接事件处理函数
        ConnectEventHandler(newfd);
      }
      else if(events[i].events & EPOLLIN){
        //读事件就绪
        ReadEventHandler(events[i].data.fd);
      }
      else if(events[i].events & EPOLLOUT){
        //写事件就绪
        WriteEventHandler(events[i].data.fd);
      }
      else if(events[i].events & EPOLLERR){
        //对应描述符发生错误
        ErrorDebug("错误事件: %d - %d",events[i].events,i);
      }
      else{
        //其他未知错误事件
        ErrorDebug("未知事件：%d",events[i].events);
      }
    }
  }
}
void EpollServer::Forwording(Channel* clientChannel, Channel* serverChannel){
  
  const size_t LEN = 4096;
  char buffer[LEN];
  int recv_len = recv(clientChannel->_fd,buffer,LEN);

  if(recv_len > 0){
    int sed_len = send(serverChannel->_fd,buffer,recv_len);
    if(sed_len < recv_len){
      //说明有一部分数据没有发送出去
      
      SendInLoop(); //将围殴发送的数据交给事件循环函数处理
    }
    else if(recv_len == 0){ //表示对方关闭

    }
    else if(recv_len < 0){ //表示传输超时

    }
  }
}
