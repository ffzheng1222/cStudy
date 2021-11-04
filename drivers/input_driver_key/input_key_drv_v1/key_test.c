


#include <stdio.h>
#include <stdlib.h>
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
	}
    }

    close(fd);

    return 0;
}






