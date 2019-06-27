#ifndef _SHOW_BMP_ANY_
#define _SHOW_BMP_ANY_

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <pthread.h>
#include <stdlib.h>

//函数声明
int show_bmp_any(char *pathname1, int x_refe, int y_refe, int wide, int high);

//相关模板声明或变量声明

#endif
