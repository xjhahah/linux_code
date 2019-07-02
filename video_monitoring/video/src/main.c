#include <stdio.h>
#include "../include/bmp.h"
#include "../include/touch_screen.h"
#include "../include/camera.h"

/*
void turn_video(){
    int t=10000;
    int i=0;
    char path[40]={0};
    while(i!=44)
    {
        sprintf(path,"./%d.jpg",i);
        i++;
        show_bmp(path);
    }
}
*/
int main(int argc, char *argv[])
{
    turn_video();
    open_ts();
    show_bmp800_480("./video.bmp");
    my_camera();
    close_ts();
    return 0;
}
