#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

//./client ip port
int main(int argc,char* argv[])
{
  if(argc!=3)
  {
    printf("Usage: %s ip port\n",argv[0]);
    return 1;
  }
  //创建套接字
  int sock=socket(AF_INET,SOCK_STREAM,0);
  if(sock<0)
  {
    perror("socket false");
    return 2;
  }
  //创建端口号
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(argv[2]));
  server.sin_addr.s_addr = inet_addr(argv[1]);

  //这里不需要监听，因为我是去连服务器的
  //connect  返回值成功返回0 失败返回-1
  if(connect(sock,(struct sockaddr*)&server,sizeof(server))<0)
  {
    perror("connect false");
    return 3;
  }

  //连接成功
  printf("connect success...\n");

  char buf[128];
  while(1)
  {
    printf("Please Enter Message: ");
    fflush(stdout);
    ssize_t s=read(0,buf,sizeof(buf)-1);  //从0 开始读去 sizeof-1 个字节的内容
    if(s > 0)
    {
      buf[s]=0;
      //发消息
      write(sock,buf,strlen(buf));

      ssize_t _s = read(sock,buf,sizeof(buf)-1);    //TCP叫做全双工

      if(_s > 0)
      {
        buf[_s]=0;
        printf("server echo# %s\n",buf);
      }
    }
    //客户端不需要accept
  }
  return 0;
}

