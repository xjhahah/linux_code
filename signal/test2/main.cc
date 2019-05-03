#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <cstdlib>

void handler(int sig){
  pid_t id = waitpid(-1,NULL,WNOHANG);
  while(id > 0){
    std::cout << "wait child success...id is : " << id << std::endl;
  }
  std::cout << "child is quit..." << getpid() << std::endl;
}
int main(){
  signal(SIGCHLD,handler);
  pid_t pid = fork();
  if(pid == 0){
    std::cout << "child id is : " << pid << std::endl;
    sleep(3);
    exit(1);
  }
  while(1){
    sleep(1);
    std::cout << "father proc do something..." << std::endl;
  }
  return 0;
}

