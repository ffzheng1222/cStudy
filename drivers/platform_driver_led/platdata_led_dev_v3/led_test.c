#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main(void)
{
    int fd;

    fd = open("/dev/led0",O_RDWR);
    if(fd < 0)
    {
	perror("open");
	exit(1);
    }

    //点灯 ---- 1
    int on;
    while(1)
    {
	on = 1;
	write(fd,&on,sizeof(on));
	sleep(1);

	on = 0;
	write(fd,&on,sizeof(on));
	sleep(1);
    }

    close(fd);
    return 0;
}
