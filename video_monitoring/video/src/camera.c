#include <stdio.h>   	//printf scanf
#include <fcntl.h>		//open write read lseek close  	 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>

#include "../include/camera.h"
#include "./jpeg/api_v4l2.h"
#include "jpeglib.h"

static unsigned char g_color_buf[FB_SIZE]={0};

int ts_x, ts_y;
int flag = 1;

int  lcd_fd;
int *mmap_fd;
struct jpg_data video_buf;//����ṹ�����

//��ʼ��LCD
int lcd_open(void)
{
	lcd_fd = open("/dev/fb0", O_RDWR);
	
	if(lcd_fd<0)
	{
			printf("open lcd error\n");
			return -1;
	}
	return 0;
}

int mmap_lcd(void)
{
	mmap_fd  = (int *)mmap(	NULL, 					//ӳ�����Ŀ�ʼ��ַ������ΪNULLʱ��ʾ��ϵͳ����ӳ��������ʼ��ַ
									FB_SIZE, 				//ӳ�����ĳ���
									PROT_READ|PROT_WRITE, 	//���ݿ��Ա���ȡ��д��
									MAP_SHARED,				//�����ڴ�
									lcd_fd, 				//��Ч���ļ�������
									0						//��ӳ��������ݵ����
	);
	return lcd_fd;

}

//LCD����
void lcd_draw_point(unsigned int x,unsigned int y, unsigned int color)
{
	*(mmap_fd+y*800+x)=color;
}

//��ʾ����ͷ��׽
int show_video_data(unsigned int x,unsigned int y,char *pjpg_buf,unsigned int jpg_buf_size)  
{
	/*���������󣬴��������*/
	struct 	jpeg_decompress_struct 	cinfo;
	struct 	jpeg_error_mgr 			jerr;	
	
	unsigned char 	*pcolor_buf = g_color_buf;
	char 	*pjpg;
	
	unsigned int 	i=0;
	unsigned int	color =0;
	//unsigned int	count =0;
	
	unsigned int 	x_s = x;
	unsigned int 	x_e ;	
	unsigned int 	y_e ;
	
	pjpg = pjpg_buf;

	/*ע�������*/
	cinfo.err = jpeg_std_error(&jerr);

	/*��������*/
	jpeg_create_decompress(&cinfo);

	/*ֱ�ӽ����ڴ�����*/		
	jpeg_mem_src(&cinfo,pjpg,jpg_buf_size);
	
	/*���ļ�ͷ*/
	jpeg_read_header(&cinfo, TRUE);

	/*��ʼ����*/
	jpeg_start_decompress(&cinfo);	
	
	x_e	= x_s+cinfo.output_width;
	y_e	= y  +cinfo.output_height;	

	/*����������*/
	while(cinfo.output_scanline < cinfo.output_height )
	{		
		pcolor_buf = g_color_buf;
			
		/* ��ȡjpgһ�е�rgbֵ */
		jpeg_read_scanlines(&cinfo,&pcolor_buf,1);
		
		for(i=0; i<cinfo.output_width; i++)
		{
			/* ��ȡrgbֵ */
			color = 		*(pcolor_buf+2);
			color = color | *(pcolor_buf+1)<<8;
			color = color | *(pcolor_buf)<<16;
			
			/* ��ʾ���ص� */
			lcd_draw_point(x,y,color);
			
			pcolor_buf +=3;
			
			x++;
		}
		
		/* ���� */
		y++;			
		
		x = x_s;
		
	}		
			
	/*�������*/
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return 0;
}

//��ʾ����jpgͼƬ
int lcd_draw_jpg(unsigned int x,unsigned int y,const char *pjpg_path)  
{
	/*���������󣬴��������*/
	struct 	jpeg_decompress_struct 	cinfo;
	struct 	jpeg_error_mgr 			jerr;	
	
	unsigned char 	*pcolor_buf = g_color_buf;
	char 	*pjpg;
	
	unsigned int 	i=0;
	unsigned int	color =0;
	//unsigned int	count =0;
	
	unsigned int 	x_s = x;
	unsigned int 	x_e ;	
	unsigned int 	y_e ;
	unsigned int	y_n	= y;
	unsigned int	x_n	= x;
	
			 int	jpg_fd;
	unsigned int 	jpg_size;

	if(pjpg_path!=NULL)
	{
		/* ����jpg��Դ��Ȩ�޿ɶ���д */	
		jpg_fd=open(pjpg_path,O_RDWR);
		
		if(jpg_fd == -1)
		{
		   printf("open %s error\n",pjpg_path);
		   
		   return -1;	
		}	
		
		/* ��ȡjpg�ļ��Ĵ�С */
		jpg_size=file_size_get(pjpg_path);	
		if(jpg_size<3000)
			return -1;
		
		/* Ϊjpg�ļ������ڴ�ռ� */	
		pjpg = malloc(jpg_size);

		/* ��ȡjpg�ļ��������ݵ��ڴ� */		
		read(jpg_fd,pjpg,jpg_size);
	}
	else
		return -1;

	/*ע�������*/
	cinfo.err = jpeg_std_error(&jerr);

	/*��������*/
	jpeg_create_decompress(&cinfo);

	/*ֱ�ӽ����ڴ�����*/		
	jpeg_mem_src(&cinfo,pjpg,jpg_size);
	
	/*���ļ�ͷ*/
	jpeg_read_header(&cinfo, TRUE);

	/*��ʼ����*/
	jpeg_start_decompress(&cinfo);	
	
	
	x_e	= x_s +cinfo.output_width;
	y_e	= y  +cinfo.output_height;	

	/*����������*/
	while(cinfo.output_scanline < cinfo.output_height )
	{		
		pcolor_buf = g_color_buf;
		
		/* ��ȡjpgһ�е�rgbֵ */
		jpeg_read_scanlines(&cinfo,&pcolor_buf,1);
		
		for(i=0; i<cinfo.output_width; i++)
		{	
			/* ��ȡrgbֵ */
			color = 		*(pcolor_buf+2);
			color = color | *(pcolor_buf+1)<<8;
			color = color | *(pcolor_buf)<<16;	
			
			/* ��ʾ���ص� */
			lcd_draw_point(x_n,y_n,color);
			
			pcolor_buf +=3;
			
			x_n++;
		}
		
		/* ���� */
		y_n++;			
		
		x_n = x_s;		
	}		
			
	/*�������*/
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	if(pjpg_path!=NULL)
	{
		/* �ر�jpg�ļ� */
		close(jpg_fd);	
		
		/* �ͷ�jpg�ļ��ڴ�ռ� */
		free(pjpg);		
	}

	return 0;
}

//LCD�ر�
void lcd_close(void)
{
	
	/* ȡ���ڴ�ӳ�� */
	munmap(mmap_fd, FB_SIZE);
	
	/* �ر�LCD�豸 */
	close(lcd_fd);
}

//��ȡjpg�ļ��Ĵ�С
unsigned long file_size_get(const char *pfile_path)
{
	unsigned long filesize = -1;	
	struct stat statbuff;
	
	if(stat(pfile_path, &statbuff) < 0)
	{
		return filesize;
	}
	else
	{
		filesize = statbuff.st_size;
	}
	
	return filesize;
}

void *start_routine(void *arg)
{
	while(flag)
	{
		read_ts(&ts_x, &ts_y);
		printf("==(x,y):(%d, %d)\n", ts_x, ts_y);
	}	
}

int my_camera(void)
{
	int fd_jpg = -1;
	int cnt = 0;
	char path[21] = {0};
	lcd_open();//��LCD��Ļ	
	mmap_lcd();//����ӳ���ϵ����DDR3�д洢������ͷ�ɼ�������ӳ�䵽LCD��Ļ����ʾ��
	
	pthread_t ts_thread;
	pthread_create(&ts_thread, NULL, start_routine, NULL);
	
	linux_v4l2_yuyv_init("/dev/video7");//��ʼ������ͷ
	linux_v4l2_start_yuyv_capturing();//��������ͷ��׽
	
	while(1)
	{
		if(ts_x>660 && ts_x<790 && ts_y>10 && ts_y<60)//ʵʱ���
		{
			linux_v4l2_get_yuyv_data(&video_buf);//��ȡ����ͷ��׽�Ļ���	
			show_video_data(0, 0, video_buf.jpg_data, video_buf.jpg_size);//��ʾ����ͷ��׽�Ļ���
		}
		
		if(x > 100 && x < 600 && y > 100 && y < 400)  //¼��
		{
			linux_v4l2_get_yuyv_data(&video_buf);//��ȡ����ͷ��׽�Ļ���
			show_video_data(0, 0, video_buf.jpg_data, video_buf.jpg_size);//��ʾ����ͷ��׽�Ļ���
			
			cnt++;
			sprintf(path, "../image/%d.jpg", cnt);
			
			fd_jpg = open(path, O_RDWR|O_CREAT|O_TRUNC);
			if(fd_jpg == -1)
			{
				printf("creat error\n");
				printf("video error\n");
			}
			write(fd_jpg, video_buf.jpg_data, video_buf.jpg_size);
			colse(fd_jpg);
			
		}
		
		if(x>150 && x<500 && y>150 && y<350)  //����¼�Ƶ�ͼ��
		{
			//����ͼƬ������Ƶ��ͼƬ����������cnt
            int num=0;
            while(num != cnt){
                show_jpeg(path);
                num++;
            }
			
		}
			
		
		if(ts_x>660 && ts_x<790 && ts_y>410 && ts_y<480)//�˳����
		{
			printf("\n======�˳����======\n");
			flag = 0;
			break;
		}
	}
	
	//�ر�����ͷ
	linux_v4l2_yuyv_quit();
	lcd_close();
	return 0;
}
