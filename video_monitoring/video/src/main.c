#include <stdio.h>
#include "../include/bmp.h"
#include "../include/touch_screen.h"
#include "../include/camera.h"

int main(int argc, char *argv[])
{
    open_ts();
    int i=1;
    char path[40]={0};
    while(1){
        sprintf(path,"../image/%d.jpg",i);

        if(i==34)
            i=1;
        show_bmp800_480(path);

        my_camera();
    }
    close_ts();
    return 0;
}
