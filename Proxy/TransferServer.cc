#include "Epoll.h"

//通道服务器---提供转发服务
class TransferServer : public EpollServer
{
  public:
    TransferServer(const char* socks5ServerIp, int socks5ServerPort, int selfPort = 8080)
      :EpollServer(selfPort)
    {}
  private:

};
