//头文件
#include "../include/show_bmp.h"



//显示全屏图片的函数
int show_bmp(char *pathname)
{
	int lcd_fd;
	int bmp_fd;
	int i;

	char bmp_buffer[800*480*3];
	int lcd_buffer[800*480];

	//打开图片
	bmp_fd = open(pathname,O_RDONLY);
	if(bmp_fd == -1)
	{
		printf("opne bmp failed!\n");
		return -1;
	}
	//打开屏幕
	lcd_fd = open("/dev/fb0",O_RDWR);
	if(lcd_fd == -1)
	{
		printf("open lcd failed!\n");
		close(bmp_fd);
		return -1;
	}

	//跳过bmp图片54个字节头
	lseek(bmp_fd,54,SEEK_SET);
	//将图片的RGB读到bup_buffer中
	read(bmp_fd,bmp_buffer,sizeof(bmp_buffer));

	//将图片的BGR合成int类型（ARGB），即一个点
	for(i=0;i<800*480;i++)
		lcd_buffer[i] = bmp_buffer[3*i+2]<<16|bmp_buffer[3*i+1]<<8|bmp_buffer[3*i]<<0 ;
		//						red 					green					blue

	//翻转
	int tmp_buffer[800*480];
	int x, y;
	for(y=0;y<480;y++)
		for(x=0;x<800;x++)
			tmp_buffer[(479-y)*800+x] = lcd_buffer[y*800+x];


	//将转换后的数据写入设备文件
	//write(lcd_fd,tmp_buffer,sizeof(tmp_buffer));

	//创建映射关系
	int *mmap_bmp = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED,lcd_fd, 0);
	if(mmap_bmp == NULL)
	{
		printf("mmap err!\n");
		return -1;

	}

	//将数据映射到设备文件
	for(i=0; i<800*480; i++)
		*(mmap_bmp+i) = tmp_buffer[i];


	//关闭LCD
	close(lcd_fd);
	//关闭BMP
	close(bmp_fd);

	//解除映射
	munmap(mmap_bmp, 800*480*4);

	return 0;
}



