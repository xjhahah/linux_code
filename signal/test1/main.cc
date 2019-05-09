#include <iostream>
#include <signal.h>
#include <unistd.h>

void handler(int sig){
    std::cout << "catch a sig: "<< sig << std::endl;
}
int main()
{
    signal(SIGSEGV,handler);
    int* p =nullptr;
    *p=20;
    while(1){
      sleep(1);
    };
    return 0;
}
