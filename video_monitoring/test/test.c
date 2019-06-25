#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

const int MAXSIZE = 800*480;

int main()
{
  //1.打开设备文件
  int lcd_fd = open("/dev/fb0",O_RDWR);
 
  //2.set color
  int buf[480][800]={0};
  int i=0;
  for(int i=0;i<800;++i){
	int j=0;
	int tmp=0;
	if(i<800/3){
		tmp=0xff0000;
	}
	else if(i<800/3*2){
		tmp = 0x00ff00;
	}
	else{
		tmp=0x0000ff;
	}
	for(j=0;j<480;++j){
		buf[j][i] = tmp;
	}
  }


  //3.write data 
  ssize_t s = write(lcd_fd,buf,800*480*4);

  //TODO

  //close fd 
  close(lcd_fd);
  return 0;
}
