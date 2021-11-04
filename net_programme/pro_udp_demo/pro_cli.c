#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

#include <arpa/inet.h>

/*通过man 7 ip 得到的头文件*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>


#define SERV_IP   "192.168.7.89"	//服务器的ip地址
#define SERV_PORT 5002	    //服务器的端口号
#define QUIT_STR  "quit"

void sys_err(char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
    int udp_fd = -1;
    int ret = -1;

    /*1. 创建套接字*/
    udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_fd < 0)
	sys_err("socket failed!");

    /*2. 使能广播*/
    int on = 1;
    ret = setsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST, (void *)&on, sizeof(on));
    if(ret < 0)
	sys_err("setsockopt failed!");


    struct sockaddr_in srv_ip;

    srv_ip.sin_family = AF_INET;
    srv_ip.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, "255.255.255.255", &srv_ip.sin_addr.s_addr);

    char kbuf[BUFSIZ];

    while(1)
    {
	bzero(kbuf, BUFSIZ);
	if(fgets(kbuf, BUFSIZ-1, stdin) ==  NULL)
	    sys_err("fgets failed!");

	//3. 发送数据
	ret = sendto(udp_fd, kbuf, BUFSIZ-1, 0, (struct sockaddr *)&srv_ip, sizeof(srv_ip));
	if(ret < 0)
	{
	    printf("%s sendto failed!\n", kbuf);
	    continue;
	}
	
	if(!strncasecmp(kbuf ,QUIT_STR ,strlen(QUIT_STR)))  //用户输入quit退出
	    break;
    }

    close(udp_fd);
    return 0;
}
