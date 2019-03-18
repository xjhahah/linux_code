#include <iostream>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int StartUp(int port)
{
  int sock = socket(AF_INET,SOCK_STREAM,0);
  struct sockaddr_in local;

  local.sin_addr.s_addr = htonl(INADDR_ANY);
}
int main()
{
  int listen_sock = StartUp(8888);
  return 0;
}
