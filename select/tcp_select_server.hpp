#pragma once 

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <sys/select.h>

#include "TcpSocket.hpp"

typedef std::function<void (const std::string* req,std::string* resp)> Handler;

class Selector{
  public:
    Selector(){
      _max_fd = 0;
      //必须初始化
      FD_ZERO(&_read_fds);
    }
    //向select中添加事件
    bool Add(TcpSocket& sock){
      int fd = sock.GetFd();
      std::cout << "[Selector::Add]: " << fd << std::endl;
      if(_fd_map.find(fd) != _fd_map.end()){
        std::cout << "Add error" << std::endl;
        return false;
      }
      _fd_map[fd] = sock;
      FD_SET(fd,&_read_fds);
      if(fd > _max_fd){
        _max_fd = fd;
      }
    return true;
    }
    //清除事件未发生改变的对应fd
    bool Del(TcpSocket& sock){
      int fd = sock.GetFd();
      std::cout << "[Selector::Del]: " << fd <<std::endl;
      if(_fd_map.find(fd) == _fd_map.end()){
        std::cout << "fd is not found" << std::endl;
        return false;
      }
      _fd_map.erase(fd);
      FD_CLR(fd,&_read_fds);

      //查找新的最大值  位图中的最大的哪一个方便下一次select调用
      for(int i=_max_fd;i >= 0;--i){
        if(!FD_ISSET(i,&_read_fds)){
          continue;
        }
        _max_fd = i;
        break;
      }
    }

    //返回就绪的fd
    bool Wait(std::vector<TcpSocket>* output){
      output->clear();

      //此处必须创建一个临时变量，否则原来的结果会被覆盖掉
      fd_set tmp = _read_fds;
      int nfds = select(_max_fd+1,&tmp,NULL,NULL,NULL);
      if(nfds < 0){
        std::cerr << "select error" << std::endl;
        return false;
      }
      //将就需的fd push_back到vector中
      for(int i=0;i < _max_fd+1; ++i){
        if(!FD_ISSET(i,&tmp)){
          continue;
        }
        output->push_back(_fd_map[i]);
      }
    }
  private:
    fd_set _read_fds;
    int _max_fd;
    std::unordered_map<int,TcpSocket> _fd_map;
}; //end Selector

class SelectServer{
  public:
    SelectServer(uint16_t port,std::string& ip)
      :_port(port)
       ,_ip(ip)
  {}
    bool Start(Handler handler)const{
      //1.创建套接字
      TcpSocket listen_sock;
      bool ret = listen_sock.Socket();
      if(!ret){
        return false;
      }
      //2.绑定端口号
      ret = listen_sock.Bind(_ip,_port);
      if(!ret){
        return false;
      }
      //3.进行监听
      ret = listen_sock.Listen(5);
      if(!ret){
        return false;
      }
      //4.创建Selector对象
      Selector selector;
      selector.Add(listen_sock);
      //5.进入事件循环
      while(1){
        std::vector<TcpSocket> output;
        bool ret = selector.Wait(&output);
        if(!ret){
          continue;
        }

        //6.根据就需文件的差异，决定后续的处理逻辑
        for(int i=0;i < output.size(); ++i){
          if(output[i].GetFd() == listen_sock.GetFd()){
            //如果是socket 就执行accept ，并接入到select中
            TcpSocket new_sock;
            listen_sock.Accept(&new_sock,nullptr,nullptr);
            selector.Add(new_sock);
          }
          else{
            //如果就绪fd是new_sock，就进行一次请求处理
            std::string req,resp;
            bool ret = output[i].Recv(&req);
            if(!ret){
              selector.Del(output[i]);
              output[i].Close();
              continue;
            }
            //调用业务函数进行计算响应
            handler(req,&resp);
            //将结果写回客户端
            output[i].Send(resp);
          }
      }
        return true;
    }
  private:
    uint16_t _port;
    std::string _ip;
}; //end SelectServer 

