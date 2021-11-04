#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h> 

#include <arpa/inet.h>

/* 下面3个头文件是通过man 7 ip得到的*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 

#define SERV_PORT 5001
#define QUIT_STR "quit"


void sys_err(char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
    int group_fd = -1;
    int ret = -1;
    struct sockaddr_in sin;

    /*1. 创建套接字 */
    group_fd = socket(AF_INET, SOCK_DGRAM, 0); //建立IPV4 TCP通信
    if(group_fd < 0) 
	sys_err("socket failed!");

    /*2. 地址重用*/
    int on = 1;
    ret = setsockopt(group_fd, SOL_SOCKET, SO_REUSEADDR, \
	    (void *)&on, sizeof(on));
    if(ret < 0)
	sys_err("setsockopt failed!");

    /*3. 绑定自己的ip地址*/
    //3.1 填写网络通信三要素的IP地址和端口号 
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port =  htons(SERV_PORT);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    //3.2 绑定指定服务端自己的ip地址 
    if( bind(group_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	sys_err("bind failed!");


    /*4. 把当前ip加入到组播当中去*/
    struct ip_mreq group;
    bzero(&group, sizeof(group));

    group.imr_multiaddr.s_addr = inet_addr("225.22.22.22");
    group.imr_interface.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, "192.168.7.89", &group.imr_interface.s_addr);
    ret = setsockopt(group_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, \
	    &group, sizeof(group));
    if(ret < 0)
	sys_err("setsockopt failed!");

    struct sockaddr_in cli_addr;
    socklen_t len = sizeof(cli_addr);

    char buf[BUFSIZ];
    char udp_cli_ip[16];

    while(1)
    {
	bzero(buf, BUFSIZ);
	//接收数据
	ret = recvfrom(group_fd, buf, BUFSIZ-1, 0, (struct sockaddr *)\
		&cli_addr, &len);
	if(ret < 0)
	{
	    printf("recive failed!\n");
	    continue;
	}
	if(ret == 0)
	{
	    printf("server said：client data recive complete!");
	    continue;
	}
	if(ret > 0)
	{
	    bzero(udp_cli_ip, 16);
	    printf("have client sent data!!!\n");
	    printf("client ip:%s，port:%d\n", inet_ntop(AF_INET, \
		  &cli_addr.sin_addr, udp_cli_ip, sizeof(cli_addr)), \
		  ntohs(cli_addr.sin_port));
	    printf("group-client said：%s\n", buf);    
	}

	if(!strncasecmp(buf, QUIT_STR, strlen(QUIT_STR)))
	    //如果用户输入了quit，则退出
	    continue;
    }

    close(group_fd);
    return 0;
}
