#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>

//TCP 服务器
int main(int argc,char* argv[])
{
  if(argc!=3)
  {
    printf("Usage: %s ip port\n",argv[0]);
    return 1;
  }
  //创建套接字
  int listen_sock=socket(AF_INET,SOCK_STREAM,0);
  if(listen_sock<0)
  {
    perror("socket false");
    return 2;
  }
  //创建端口号
  struct sockaddr_in local;
  local.sin_family = AF_INET;
  local.sin_port = htons(atoi(argv[2]));
  local.sin_addr.s_addr = inet_addr(argv[1]);


  //检测绑定是否完成
  if(bind(listen_sock,(struct sockaddr*)&local,sizeof(local))<0)
  {
    perror("bind false");
    return 3;
  }

  //TCP 面向连接
  //将套接字设置为监听状态  不断地获取新连接
  //backlog   等待队列长度,不能太大也不能太长
  
  if(listen(listen_sock,10)<0)
  {
    perror("listen false");
    return 4;
  }
  
  while(1)
  {
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    //accept 获取连接 成功返回套接字 失败返回-1
    
    int new_sock = accept(listen_sock,(struct sockaddr*)&client,&len);
  
    if(new_sock < 0)
    {
      perror("accept false");
      continue;
    }

    //走到这里说明获的了一个新的链接
    printf("get a new client:[%s:%d]\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));

    pid_t id = fork();

    if(id==0)
    {
      close(listen_sock);  //子进程看不到套接字  关闭
      if(fork() > 0)
      {
        exit(0);
      }
      char buf[128];
      while(1)                                                                                      
      {                                                                                                                       
        ssize_t s = read(new_sock,buf,sizeof(buf)-1);                           
        if(s > 0)                                                                               
        {                                                                               
          buf[s]=0;                                                                       
          printf("[%s:%d]:> %s \n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),buf);      
          write(new_sock,buf,strlen(buf));  //向buf写 strlen 个字符                 
        }                                                                         
        //如果read返回值为0 表示 链接（客户端） 关闭                                          
        else if(s==0)                                             
        {                                                                     
          printf("client quit , break！\n");              
          break;                                                              
        }                                               
        else    //打开失败                                                                
        {                                                               
          perror("read false");                                                     
          break;                                                          
        }       
      } 
      close(new_sock);
      exit(0);
    }
    else 
    {
      close(new_sock);       //必须做这一步，否则会导致父进程资源越来越少，文件描述符资源泄漏

      waitpid(id,NULL,0);   //必须等待子进程，否则会产生僵尸状态 可以用 SINCHILD 来忽略子进程推出状态
    }
  }
  return 0;
}

