#pragma once
#include "Header.h"

class EpollServer 
{
 public:
  EpollServer(int port) 
	  : _port(port)
	  , _listenfd(-1)
	  , _eventfd(-1)
  {}

  //启动服务，开始监听
  void Start();

  //事件循环
  void EventLoop();



  virtual ~EpollServer() { 
	  if (_listenfd != -1) {
		  close(_listenfd);
	  }
	  //_eventfd
  }
 protected:
	 int _listenfd; //监听套接字
	 int _port; //端口号
  
	 int _event;  //epoll事件
	 int _eventfd; //事件文件描述符
	 static const size_t MAX_EVENT; //最大的事件数
};
