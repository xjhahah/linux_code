#pragma once

#include "Epoll.h"

class TransferServer : public EpollServer
{
  public:
    TransferServer(const char* socks5ServerIp,int socks5ServerPort,int selfPort = 8080)
      :EpollServer(selfPort)
    {
      memset(&_socks5addr,0,sizeof(struct sockaddr_in));
      _socks5addr.sin_family = AF_INET;
      _socks5addr.sin_port = htons(socks5ServerPort);
      _socks5addr.sin_addr.s_addr = inet_addr(socks5ServerIp);
    }

    virtual void ConnectEventHandle(int connectfd);
    virtual void ReadEventHandle(int connectfd);
    virtual void WriteEventHandle(int connectfd);

  protected:
    struct sockaddr_in _socks5addr;   //server addr
};

