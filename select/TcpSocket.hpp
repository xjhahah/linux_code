#pragma once 

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string>
#include <unistd.h>

class TcpSocket{
  public:
    TcpSocket()
      :_sock(-1)
  {}
    //创建套接字
    bool Socket(uint16_t port,int _listen_num){
      _sock = socket(AF_INET,SOCK_STREAM,0);
      if(_sock < 0){
        std::cerr << "create socket error" << std::endl;
        return false;
      }
      return true;
    }
    bool Bind(std::string ip,uint16_t port){
      int opt = 1;
      size_t ret = setsockopt(_sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
      if(ret < 0){
        std::cerr << "setsockopt error" << std::endl;
        exit(2);
      }
      struct sockaddr_in listen_sock;
      listen_sock.sin_family = AF_INET;
      listen_sock.sin_port = htons(port);
      listen_sock.sin_addr.s_addr = INADDR_ANY;

      if(bind(_sock,(struct sockaddr*)&listen_sock,sizeof(listen_sock)) < 0){
        std::cerr << "bind error!" << std::endl;
        return false;
      }
      return true;
    }
    bool Listen(){
      if(listen(_sock,_listen_num) < 0){
        std::cerr <<"listen error" << std::endl;
        return false;
      }
      return true;
    }
    /*
    int Accept(TcpSocket* sock,void*(),void*()){
      struct sockaddr_in addr;
      socklen_t len = sizeof(addr);
      int fd = accept(sock->_sock,(struct sockaddr*)&addr,&len);
      if(fd < 0){
        std::cerr << "accept error" << std::endl;
        return -1;
      }
      return fd;
    }
    bool Send(std::string& resp){

      ssize_t ret = recv(_sock,resp.c_str(),resp.size(),0);
      if(ret < 0){
        std::cout << "recv error" << std::endl;
        return false;
      }
      std::cout << resp << std::endl;
      Close();
      return true;
    }
    bool Recv(std::string& req){
        req = "HTTP/1.1 200 OK\r\n\r\n<html><body><h1>select</h1></body></html>";
        write(_sock,req.c_str(),req.size());
        Close();
    }
    */ 
    int GetFd(){
      return _sock;
    }
    void Close()
    {
      close(_sock);
    }
  private:
    int _sock;
    uint16_t port;
    int _listen_num;
}; //end Socket
