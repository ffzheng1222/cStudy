


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>



int main(int argc, char *argv[])
{
    int ret = -1;
    int fd;

    //用户空间读取输入设备的数据包-----统一格式
    struct input_event event;


    fd = open("/dev/event0", O_RDWR);
    if(fd < 0)
    {
	perror("open failed!\n");
	exit(1);
    }

    while(1)
    {
	ret = read(fd, &event, sizeof(event));
	if(ret < 0)
	{
	    perror("open failed!\n");
	    exit(1);
	}
	if(event.type == EV_KEY)
	{
	    if(event.code == KEY_DOWN)
	    {
		if(event.value){
		    //按键按下
		    printf("<APP>------KEY_DOWN is pressed...\n");
		}else{
		    //按键抬起
		    printf("<APP>------KEY_DOWN is up...\n");			
		}

	    }
	    if(event.code == KEY_UP)
	    {
		if(event.value){
		    //按键按下
		    printf("<APP>------KEY_UP is pressed...\n");
		}else{
		    //按键抬起
		    printf("<APP>------KEY_UP is up...\n");			
		}

	    }
	    if(event.code == KEY_LEFT)
	    {
		if(event.value){
		    //按键按下
		    printf("<APP>------KEY_LEFT is pressed...\n");
		}else{
		    //按键抬起
		    printf("<APP>------KEY_LEFT is up...\n");			
		}

	    }
	    if(event.code == KEY_RIGHT)
	    {
		if(event.value){
		    //按键按下
		    printf("<APP>------KEY_RIGHT is pressed...\n");
		}else{
		    //按键抬起
		    printf("<APP>------KEY_RIGHT is up...\n");			
		}

	    }
	    if(event.code == KEY_ENTER)
	    {
		if(event.value){
		    //按键按下
		    printf("<APP>------KEY_ENTER is pressed...\n");
		}else{
		    //按键抬起
		    printf("<APP>------KEY_ENTER is up...\n");			
		}

	    }
	    if(event.code == KEY_ESC)
	    {
		if(event.value){
		    //按键按下
		    printf("<APP>------KEY_ESC is pressed...\n");
		}else{
		    //按键抬起
		    printf("<APP>------KEY_ESC is up...\n");			
		}

	    }
	    if(event.code == KEY_HOME)
	    {
		if(event.value){
		    //按键按下
		    printf("<APP>------KEY_HOME is pressed...\n");
		}else{
		    //按键抬起
		    printf("<APP>------KEY_HOME is up...\n");			
		}

	    }
	    if(event.code == KEY_POWER)
	    {
		if(event.value){
		    //按键按下
		    printf("<APP>------KEY_POWER is pressed...\n");
		}else{
		    //按键抬起
		    printf("<APP>------KEY_POWER is up...\n");			
		}

	    }
	}
    }

    close(fd);

    return 0;
}






