#include "TcpSocket.hpp"
#include "tcp_select_server.hpp"

using std::cout;
using std::endl;
using std::cerr;

#define SIZE sizeof(fd_set)*8 //fd_set最多能够记录的文件描述符的多少

bool SetNoBlock(int fd){
  int fl = fcntl(fd, F_GETFL);
  if(fl < 0) {
    return false;
  }
  int ret = fcntl(fd, F_SETFL, O_NONBLOCK);
  if(ret < 0) {
    return false;
  }

  return true;
}

int main(int argc, char* argv[])
{
  //SelectSever sec;
  Selector sec;
  TcpSocket server_sock;
  if(argc < 2)
  {
    cerr << "[./select] [port]" << endl;
  }
  if(server_sock.Socket(atoi(argv[1]), 5) == false) {
    return -1;
  }
  sec.Add(server_sock);

  for(;;) {
    std::vector<TcpSocket> output;
    if(!sec.Wait(&output)) {
      continue;
    }

    for(int i = 0; i < static_cast<int>(output.size()); ++i) {
      //处理响应
      struct sockaddr_in caddr;
      socklen_t len = sizeof(caddr);
      int client_fd = accept(output[i].GetFd(), (struct sockaddr*)&caddr, &len);
      char buf[4096] = {0};
      ssize_t ret = recv(client_fd, buf, sizeof(buf), 0);
      if(ret <= 0) {
        continue;
      }
      else
      {
        buf[ret] = 0;
      }

      cout << buf << endl;

      std::string resp = "HTTP/1.1 200 OK\r\n\r\n<html><body><h1>select</h1></body></html>";
      write(client_fd, resp.c_str(), resp.size());
      close(client_fd);
    }

  }


  return 0;
}
