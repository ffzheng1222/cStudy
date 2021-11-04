
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>


//自定义宏
#define LED_NUM_ON	 _IOW('L', 0x1111, int)
#define LED_NUM_OFF	 _IOW('L', 0x2222, int)
#define LED_ALL_ON	 _IO('L',  0x3333)
#define LED_ALL_OFF	 _IO('L',  0x4444)

#define LED_GPC0LED_GPC03   3
#define LED_GPC0LED_GPC04   4



void user_led_write(int fd);
void user_led_control(int fd);



int main(void)
{
    int fd;

    fd = open("/dev/led", O_RDWR);
    if(fd < 0)
    {
	perror("open failed!");
	exit(1);
    }

//    user_led_write(fd);

    user_led_control(fd);

    return 0;
}

void user_led_write(int fd)
{
    int status = 0;
    while(1)
    {
	status = 1;
	write(fd, &status, sizeof(status));
	usleep(800000);

	status = 0;
	write(fd, &status, sizeof(status));
	usleep(800000);
    }
}

void user_led_control(int fd)
{
    while(1)
    {
	ioctl(fd, LED_NUM_ON, LED_GPC0LED_GPC03); 
	usleep(500000); 
	ioctl(fd, LED_NUM_OFF, LED_GPC0LED_GPC03); 
	usleep(500000); 

	ioctl(fd, LED_NUM_ON, LED_GPC0LED_GPC04); 
	usleep(500000); 
	ioctl(fd, LED_NUM_OFF, LED_GPC0LED_GPC04); 
	usleep(500000); 

	ioctl(fd, LED_ALL_ON); 
	usleep(500000); 
	ioctl(fd, LED_ALL_OFF); 
	usleep(500000); 
    }  
}



