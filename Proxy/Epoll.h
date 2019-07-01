#pragma once
#include "Header.h"

class EpollServer 
{
 public:
  EpollServer(int port) 
	  : _port(port)
	  , _listenfd(-1)
	  , _event_fd(-1)
  {}

  //�������񣬿�ʼ����
  void Start();

  //�¼�ѭ��
  void EventLoop();

  virtual void ConnectEventHandler(int fd) = 0;
  virtual void ReadEventHandler(int fd) = 0;
  virtual void WriteEventHandler(int fd) = 0;

 
  //ͨ��
  struct Channel
  {
    int _fd;        //socket ������
    int _event;     //�¼�
    string _buffer; //ֻ��������

    Channel()
      :_fd(-1)
       ,_event(0)
    {}
  };

  //TODO  ����
  struct Connect 
  {
    Channel _clientChannel;  //�ͻ���ͨ��
    Channel _serverChannel;  //������ͨ��
  };


  //ת������
  void Forwording(Channel* clientChannel, Channel* serverChannel);

  //TODO
  void SetNonblocking(int sfd){
    int flag,s;
  }

  //ע���¼�
  void OpEvent(int fd,int op,int how,int line){ //how����Ҫ������fd
    struct epoll_event event;
    event.events = op;   //��ʾ�����������꣩
    event.data.fd = fd;  //epoll_create()����ֵ��epoll�����
    if(epoll_ctl(_event_fd,how,fd,&event) < 0){
      ErrorDebug("epoll_ctl.fd: %d + how: %d + line: %d",fd,how,line);
    }
  }

  virtual ~EpollServer() { 
	  if (_listenfd != -1) {
		  close(_listenfd);
	  }
	  //_eventfd
  }
 private:
  EpollServer(const EpollServer&);
  EpollServer& operator=(const EpollServer&);

 protected:
	 int _listenfd; //�����׽���
	 int _port; //�˿ں�
  
	 int _event;  //epoll�¼�
	 int _event_fd; //�¼��ļ�������
	 static const size_t _MAX_EVENT; //�����¼���

   map<int,Connect*> _connectMap;  //
};
