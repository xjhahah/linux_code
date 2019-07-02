#include "touch_screen.h"

int fd_ts;

int open_ts(void)//1，打开触控屏(open)
{	
	fd_ts = open("/dev/input/event0", O_RDWR);
	if(fd_ts == -1)
	{
			printf("open event0 fail!\n");
	}		
	return 0;
}


int read_ts(struct xy_sample *point)//2，读取触控屏的信息到结构体中。(read)
{
	struct input_event event;
	int cont = 0 ;
	//           read 会阻塞
	while(1)
	{        
		read( fd_ts , &event , sizeof(struct input_event));
		

		if ( EV_ABS  ==  event.type )

			if (ABS_X == event.code)
			{
				cont ++;
				point->x = event.value ;
			}

			else if (ABS_Y == event.code)
			{
				point->y = event.value ;
				cont ++;
			}

        if(event.type == EV_KEY &&  event.code == BTN_TOUCH) //事件为触摸屏按下和松开的事件
        {
            printf("event.value:%d\n", event.value);
			return event.value;			// 1为按下，0为松开
		}
		else if(cont == 2)
		{
			return -1;   //滑动
		}
	}

}

int close_ts(void)
{
	close(fd_ts);
	return 0;
}