#include "TransferServer.h"


//重写虚函数

void TransferServer::ConnectEventHandle(int connectfd){
  
  //连接sockets服务器
  int socks5fd = socket(AF_INET,SOCK_STREAM,0);
  if(socks5fd < 0){
    ErrorDebug("socks5 socket error");
    return;
  }

  //客户端连接服务器
  if(connect(socks5fd,(struct sockaddr*)&_socks5addr,sizeof(_socks5addr)) < 0){
    ErrorDebug("connect socks5 server error");
    return;
  }

  //表示连接成功,创建连接及客户端服务器通道
  Connect* con = new Connect;
  con->_clientChannel._fd = connectfd;
  con->_clientChannel._event = EPOLLIN;
  OpEvent(connectfd,con->_clientChannel._event,EPOLL_CTL_ADD,__LINE__);

  con->_serverChannel._fd = socks5fd;

  _connectMap[connectfd] = con;
  _connectMap[socks5fd] = con;
}

void TransferServer::ReadEventHandle(int fd){
  map<int,Connect*>::iterator it = _connectMap.find(fd);
  if(it != _connectMap.end()){
    Connect* con = it->second;
    Channel* clientChannel = &(con->_clientChannel);  //默认使得Channel指向client
    Channel* serverChannel = &(con->_serverChannel);

    if(fd == serverChannel->_fd){
      swap(clientChannel,serverChannel);
    }

    Forwording(clientChannel,serverChannel);  //数据转发

  }
}
