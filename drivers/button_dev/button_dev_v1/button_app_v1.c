



#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>




int main(void)
{
	int fd = open("/dev/button0", O_RDWR);
	if(fd < 0)
	{
		perror("open failed!\n");
		exit(1);
	}

	
	close(fd);

	return 0;
}
























