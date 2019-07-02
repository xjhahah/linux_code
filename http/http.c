#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

const int MAX = 1024;

//手册
static void Usage(const char* proc){
  printf("Usage: %s port\n",proc);
}


int startUp(int port)
{
  int sock = socket(AF_INET,SOCK_DGRAM,0);
  if(sock < 0){
    perror("socket error");
    exit(2);
  }

  //避免TIME_WAIT
  int opt = 1;
  setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

  struct sockaddr_in local;
  local.sin_family = AF_INET;
  local.sin_addr.s_addr = htonl(INADDR_ANY);
  local.sin_port = htons(port);

  if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0){
    perror("bind");
    exit(3);
  }

  if(listen(sock,5) < 0){
    perror("listen");
    exit(4);
  }

  return sock;
}

int getline(int sock,char line[],int len)
{

}

//线程函数
int handlerRequest(void* arg)
{
  int sock = (int)arg;

  char line[MAX];

  getline(sock,line,sizeof(line));
  close(sock);
}


//./http 8080
int main(int argc,char* argv[])
{
  if(argc != 2){
    Usage(argv[0]);
    return 1;
  }

  //监听套接字
  int listen_sock = startUp(atoi(argv[1]));

  for( ; ; ){
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int sock = accept(listen_sock,(struct sockaddr*)&client,&len);

    if(sock < 0){
      perror("accept error");
      continue;
    }

    //创建线程
    pthread_t tid;
    pthread_create(&tid,NULL,handlerRequest,(void*)tid);
    pthread_detach(tid);
  }
  return 0;
}
