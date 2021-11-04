


#include <stdio.h>
#include <stdlib.h>
#include <erron.h>
#include <unistd.h>
#include <strings.h>

#include <linux/input.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <poll.h>

#include <sys/mman.h>


#define BUTTON_IOC_GET_DATA	0x6666
#define PAGE_SIZE   1<<12


struct button_event{
    int code;
    int value;
};


struct mem_data{
    char  buf[128];
};



int main(int argc, char *argv[])
{
    int ret =  -1;
    int fd;
    char k_buf[128];
    struct button_event event;

    
    char *content = "Hello kernel, I'am fan!";
    char *addr;
    struct mem_data data;


    fd = open("/dev/button0", O_RDWR);
    if(fd < 0)
    {
	perror("open failed!\n");
	exit(1);
    }

    //1.测试mmap功能
    addr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED)
    {
	perror("mmap failed!\n");
	exit(1);
    }

    //2.通过地址写入数据
    memcpy(addr, content, strlen(content));

    sleep(1);

    //3.验证数据是否写入成功
    ret = ioctl(fd, BUTTON_IOC_GET_DATA, &data);
    if(ret < 0)
    {
	perror("ioctl failed!\n");
	exit(1);
    }

    printf("data.buf = %s\n", data.buf);

    sleep(1);



    //监控两个fd--------标准输入，当前button按键
    struct pollfd pfds[2];
    pfds[0].fd = 0;
    pfds[0].events = POLLIN;

    pfds[1].fd = fd;
    pfds[1].events = POLLIN;

    while(1)
    {
	ret = poll(&pfds, 2, -1);
	if(ret < 0)
	{
	    perror("poll failed!\n");
	    exit(1);
	}
	if(ret > 0)
	{
	    if(pfds[0].revents & POLLIN)
	    {
		if(!fgets(k_buf, 128, stdin))
		{
		    printf("<poll-APP>----k_buf = %s\n", k_buf);
		    bzero(k_buf, 128);
		}

	    }
	    if(pfds[1].revents & POLLIN)
	    {
		bzero(&event, sizeof(struct button_event));
		ret = read(pfds[1].fd, &event, sizeof(struct button_event));
		if(ret < 0)
		{
		    perror("read failed!\n");
		    exit(1);
		}

		switch(event.code)
		{
		    case KEY_UP:
		    if(event.value){
			printf("<poll-APP>----button1  upspring...\n");
		    }else{
			printf("<poll-APP>----button1 pressed...\n");
		    }
			break;
		    case KEY_DOWN:
		    if(event.value){
			printf("<poll-APP>----button2 upspring...\n");
		    }else{
			printf("<poll-APP>----button2 pressed...\n");
		    }
			break;
		    case KEY_LEFT:
		    if(event.value){
			printf("<poll-APP>----button3 upspring...\n");
		    }else{
			printf("<poll-APP>----button3 pressed...\n");
		    }
			break;
		    case KEY_RIGHT:
		    if(event.value){
			printf("<poll-APP>----button4 upspring...\n");
		    }else{
			printf("<poll-APP>----button4 pressed...\n");
		    }
			break;
		    case KEY_ENTER:
		    if(event.value){
			printf("<poll-APP>----button5 upspring...\n");
		    }else{
			printf("<poll-APP>----button5 pressed...\n");
		    }
			break;
		    case KEY_ESC:
		    if(event.value){
			printf("<poll-APP>----button6 upspring...\n");
		    }else{
			printf("<poll-APP>----button6 pressed...\n");
		    }
			break;
		    default:
			printf("unknow button!!!\n");
		}

	    }

	}
    }

    close(fd);
    munmap(addr, PAGE_SIZE);

    return 0;
}








