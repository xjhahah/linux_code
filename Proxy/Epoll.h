#pragma once
#include "Header.h"

class EpollServer 
{
 public:
  EpollServer(int port) 
	  : _port(port)
	  , _listenfd(-1)
	  , _event_fd(-1)
  {}

  //启动服务，开始监听
  void Start();

  //事件循环
  void EventLoop();

  virtual void ConnectEventHandler(int fd) = 0;
  virtual void ReadEventHandler(int fd) = 0;
  virtual void WriteEventHandler(int fd) = 0;

 
  //通道
  struct Channel
  {
    int _fd;        //socket 描述符
    int _event;     //事件
    string _buffer; //只读缓冲区

    Channel()
      :_fd(-1)
       ,_event(0)
    {}
  };

  //TODO  连接
  struct Connect 
  {
    Channel _clientChannel;  //客户端通道
    Channel _serverChannel;  //服务器通道
  };


  //转发函数
  void Forwording(Channel* clientChannel, Channel* serverChannel);

  //TODO
  void SetNonblocking(int sfd){
    int flag,s;
  }

  //注册事件
  void OpEvent(int fd,int op,int how,int line){ //how：需要监听的fd
    struct epoll_event event;
    event.events = op;   //表示动作（三个宏）
    event.data.fd = fd;  //epoll_create()返回值（epoll句柄）
    if(epoll_ctl(_event_fd,how,fd,&event) < 0){
      ErrorDebug("epoll_ctl.fd: %d + how: %d + line: %d",fd,how,line);
    }
  }

  virtual ~EpollServer() { 
	  if (_listenfd != -1) {
		  close(_listenfd);
	  }
	  //_eventfd
  }
 private:
  EpollServer(const EpollServer&);
  EpollServer& operator=(const EpollServer&);

 protected:
	 int _listenfd; //监听套接字
	 int _port; //端口号
  
	 int _event;  //epoll事件
	 int _event_fd; //事件文件描述符
	 static const size_t _MAX_EVENT; //最大的事件数

   map<int,Connect*> _connectMap;  //
};
