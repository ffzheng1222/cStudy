


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




struct button_event{
    int code;
    int value;
};


int main(int argc, char *argv[])
{
    int ret =  -1;
    int fd;
    char k_buf[128];
    struct button_event event;

    fd = open("/dev/button0", O_RDWR);
    if(fd < 0)
    {
	perror("open failed!\n");
	exit(1);
    }

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

		if(event.code == KEY_DOWN)
		{
		    if(event.value){
			printf("<poll-APP>----button2 upspring...\n");
		    }else{
			printf("<poll-APP>----button2 pressed...\n");
		    }

		}
	    }

	}
    }

    close(fd);

    return 0;
}








