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

  //�������񣬿�ʼ����
  void Start();

  //�¼�ѭ��
  void EventLoop();



  virtual ~EpollServer() { 
	  if (_listenfd != -1) {
		  close(_listenfd);
	  }
	  //_eventfd
  }
 protected:
	 int _listenfd; //�����׽���
	 int _port; //�˿ں�
  
	 int _event;  //epoll�¼�
	 int _eventfd; //�¼��ļ�������
	 static const size_t MAX_EVENT; //�����¼���
};
