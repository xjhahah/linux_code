#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

void SetNoBlock(int fd)
{
  int f1=fcntl(fd,F_GETFL);
  if(f1<0)
  {
    std::cerr << "fcntl err" << std::endl;
    exit(0);
  }
  fcntl(fd,F_SETFL, f1 | O_NONBLOCK);
}

int main()
{
  SetNoBlock(0);
  while(1)
  {
    char buf[1024] = {0};
    ssize_t size = read(0,buf,sizeof(buf)-1);
    if(size<0)
    {
      std::cerr << "read" << std::endl;
      sleep(1);
      continue;
    }
    std::cout << buf <<std::endl;
  }
  return 0;
}
