#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define I2C_SLAVE       0x0703

void print_usage(char *str)
{
    printf("%s r     : read at24c02 addresss 0\n", str);
    printf("%s w val : write at24c02 addresss 0\n", str);
}

int main(int argc,char **argv)
{
	int fd;
	unsigned char val;//�ֽ�

	char register_addr = 0x08; /* Device register to access Ƭ�ڵ�ַ*/
	int  res;
	char wbuf[10];
	char rbuf[10];

	if (argc < 2){
		print_usage(argv[0]);
		exit(1);
	}

	/*���豸�ļ�*/
	fd = open("/dev/at24_dev", O_RDWR);
	if (fd < 0) 
	{
		perror("open failed");
		exit(1);
	}
	if (strcmp(argv[1], "r") == 0){
		if (write(fd, &register_addr, 1)!=1) {
			perror("write failed");
			exit(1);
		}

		if (read(fd, rbuf, 1) != 1) {
			perror("read failed");
			exit(1);
		} else {
			printf("rbuf =0x%x\n",rbuf[0]);

		}	

	}
	else if ((strcmp(argv[1], "w") == 0) && (argc == 3))
	{
		//  ./test  w  0x99
		val = strtoul(argv[2], NULL, 0);		

		wbuf[0] = register_addr; // Ƭ�ڵ�ַ0x08
		wbuf[1] = val;

		if (write(fd, wbuf, 2)!=2) {
			perror("write failed");
			exit(1);
		}		      
		printf("write data ok!\n");

	}

	close(fd);
	return 0;
}



