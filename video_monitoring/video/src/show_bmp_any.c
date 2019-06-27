//头文件
#include "../include/show_bmp_any.h"


//显示任意位置任意大小的图片 路径		x坐标     y坐标     宽度      高度
int show_bmp_any(char *pathname1, int x_refe, int y_refe, int wide, int high)
{
	if((x_refe+wide > 800) || (y_refe+high>480))
	{
		printf("err!\n");
		return -1;
	}
		
	int lcd_fd;
	int bmp_fd;
	int i;

	
	char bmp_buffer1[wide*high*3];
	int lcd_buffer1[wide*high];
	
	
	//打开图片
	bmp_fd = open(pathname1,O_RDONLY);  
	if(bmp_fd == -1)
	{
		printf("opne %s bmp failed!\n",pathname1);
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
	read(bmp_fd,bmp_buffer1,sizeof(bmp_buffer1));
	
	//将图片的BGR合成int类型（ARGB），即一个点
	for(i=0;i<wide*high;i++)
		lcd_buffer1[i] = bmp_buffer1[3*i+2]<<16|bmp_buffer1[3*i+1]<<8|bmp_buffer1[3*i]<<0 ;
		//						red 					green					blue
	
	//翻转
	int tmp_buffer1[wide*high];
	int x, y;
	for(y=0;y<high;y++)
		for(x=0;x<wide;x++)
			tmp_buffer1[(high-y-1)*wide+x] = lcd_buffer1[y*wide+x];
	
	
	//将转换后的数据写入设备文件
	//write(lcd_fd,tmp_buffer,sizeof(tmp_buffer));
	
	//创建映射关系
	int *mmap_bmp1 = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED,lcd_fd, 0);
	if(mmap_bmp1 == NULL)
	{
		printf("mmap err!\n");
		return -1;
		
	}
	
	//将数据映射到设备文件
	for(y=0; y<high; y++)
		for(x=0;x<wide; x++)
		*(mmap_bmp1+(y+y_refe)*800+x+x_refe) = tmp_buffer1[y*wide+x];
	
	
	//关闭LCD
	close(lcd_fd);
	//关闭BMP
	close(bmp_fd);

	//解除映射
	munmap(mmap_bmp1, 800*480*4);
	
	return 0;
}
