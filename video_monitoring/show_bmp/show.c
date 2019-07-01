#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

const int MAXSIZE = 800*480*3;

int main()
{
	int lcd_fd = open("/dev/fb0",O_RDWR);
	
	int bmp_fd = open("./test.bmp",O_RDWR);
	if(bmp_fd < 0){
		printf("open file falied!!!\n");
		exit(1);
	}
	
	//set offset
	lseek(bmp_fd,54,SEEK_SET);

	//read picture info
	char buf[MAXSIZE];
	ssize_t s =  read(bmp_fd,buf,MAXSIZE);
	if(s < 0){
		printf("read failed...\n");
		exit(2);
	}
	
	//jiang yan se shuju  xiugai weizhi,  BGR ---> RGB
	int lcd_buf[800*480];

	for(int i = 0;i < 800*480; ++i){
		lcd_buf[i] = (buf[2+3*i]<<16)  | (buf[1+3*i]<<8) | buf[3*i];		
	}

	//write data
	ssize_t ss = write(lcd_fd,lcd_buf,800*480*4);

	//close fd
	close(lcd_fd);
	close(bmp_fd);
	return 0;
}
