#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <string.h>
#include <ctype.h>
const int MAX = 1024;

//手册
static void Usage(const char* proc){
  printf("Usage: %s port\n",proc);
}


int startUp(int port)
{
  int sock = socket(AF_INET,SOCK_STREAM,0);
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

//按行读取网页内容
int getLine(int sock,char line[],int len)
{
  char c='a';
  int i=0;
  while(c != '\n' && i < len - 1){
    ssize_t s = recv(sock,&c,1,0);
    if(s > 0){
      if(c == '\r'){ // \r  \r\n \n
        recv(sock,&c,1,MSG_PEEK); //将\r \r\n  转换成\n
        if(c == '\n'){
          recv(sock,&c,1,0); //如果\r下一个字符是\n。。。   
        }
        else{
           c = '\n';
        }
      }  //要么是\n要么是其他有效字符
     line[i++] = c;
    }
    else{
      break;
    }
  } 
  line[i] = '\0';

  return i;
}

void clearHeader(int sock)
{
  char line[MAX];
  do{
    getLine(sock,line,sizeof(line));
  }while(strcmp(line,"\n"));
}


void echoError(int status_code)
{
  ;
}

//线程函数
void* handlerRequest(void* arg)
{
  printf("get a new client, thread create success...\n");
  int sock = (int)arg;

  char line[MAX];
  char method[MAX/6];
  char url[MAX];
  char path[MAX];
  int i=0,j=0;
  int status_code = 200;
  int cgi = 0;
  char* query_string = NULL;

  //method url http_version
  getLine(sock,line,MAX);
  
  //提取方法
  while(i < sizeof(method) - 1 && j < sizeof(line) && !isspace(line[j])){
    method[i] = line[j];
    i++;
    j++;
  }
  method[i] = '\0';

  //GET POST 
  if(strcasecmp(method,"GET") == 0){ //strcasecmp  忽略大小写比较字符串

  }
  else if(strcasecmp(method,"POST") == 0){

  }
  else{
    //请求出错 4xx  5xx 
    //清理报头
    clearHeader(sock);
    status_code = 404;
    goto end;
  }

  //处理url
  i=0;
  while(j < sizeof(line) && isspace(line[j])){
      j++;
  }
  while(i < sizeof(url)-1 && j < sizeof(line) && !isspace(line[j])){
    url[i] = line[j];
    i++;
    j++;
  }
  url[i] = '\0';

  //url = /xx/xx/xxx?a=q000&b=100
  if(strcasecmp(method,"GET") == 0){
    query_string = url;
    while((*query_string)){
      if(*query_string == '?'){
        *query_string = '\0';
        cgi = 1;
        break;
      }
      query_string++;
    }
  }

  //说明没有带参
  
  //method url[path,query_string(GET)]

  sprintf(path,"wwwroot%s",url);
  printf("method: %s,  url: %s, query_string: %s\n",method,path,query_string);

end:
  if(status_code != 200){
    echoError(status_code);
  }

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
    pthread_create(&tid,NULL,handlerRequest,(void*)sock);
    pthread_detach(tid);
  }
  return 0;
}
