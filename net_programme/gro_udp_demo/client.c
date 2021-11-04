#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

/* 下面3个头文件是通过man 7 ip得到的*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <arpa/inet.h>

#define SERV_PORT 5001
#define QUIT_STR "quit"

void sys_err(char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
    int udp_fd = -1;
    int ret = -1;

    /* 1.创建套接字 */
    udp_fd = socket(AF_INET, SOCK_DGRAM, 0); //建立IPV4 TCP通信
    if(udp_fd < 0) {
	perror("socket");
	exit(1);
    }

    /* 2.直接发送数据*/
    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, "225.22.22.22", &srv_addr.sin_addr.s_addr);  

    char kbuf[BUFSIZ];

    while(1)
    {
	bzero(kbuf, BUFSIZ);
	if(fgets(kbuf, BUFSIZ-1, stdin) ==  NULL)
	    sys_err("fgets failed!");

	ret = sendto(udp_fd, kbuf, sizeof(kbuf), 0, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
	if(ret < 0)
	{
	    printf("%s sendto failed!\n", kbuf);
	    continue;
	}

	if(!strncasecmp(kbuf ,QUIT_STR ,strlen(QUIT_STR)))
	    break;
    }

    close(udp_fd);
    return 0;	    


}
