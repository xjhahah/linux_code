#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>


using namespace std;

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

int main()
{
  int listen_sock = StartUp(6666);

  //创建epoll模型  就绪队列 红黑树 回调函数
  int ep_fd = epoll_create(256);

  return 0;
}
