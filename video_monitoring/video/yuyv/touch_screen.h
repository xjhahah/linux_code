#ifndef __TOUCH_SCREEN_H
#define __TOUCH_SCREEN_H

#include <stdio.h>//printf
#include <linux/input.h>//struct input_event
#include <sys/types.h>//open
#include <sys/stat.h>//open
#include <fcntl.h>//open
#include <unistd.h>//read

struct xy_sample
{
    int x;
    int y;
};

int open_ts(void);
int read_ts(int *coordinate_x, int *coordinate_y);
int close_ts(void);

#endif