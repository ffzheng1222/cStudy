


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>



struct button_event{
		int code;		//按键的名字		上下左右键......
		int value;		//按键的状态		按下/抬起-------1/0	
};



int main(int argc, char *argv[])
{
	int ret = -1;
	int fd;

	struct button_event event;
	
	fd = open("/dev/button0", O_RDWR);
	if(fd < 0)
	{
		perror("open failed!\n");
		exit(1);
	}

	while(1)
	{
		if(event.code = KEY_DOWN)
		{
			if(!event.value){
				//按键按下
				printf("<APP>------button2 pressed...\n");
			}else{
				//按键抬起
				printf("<APP>------button2 upspring...\n");			
			}
		}

	}

	
	close(fd);

	return 0;
}


























