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


#define SERV_IP   "192.168.7.89"    //服务器的ip地址
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
    struct sockaddr_in sin;

    /*1. 创建套接字*/
    udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_fd < 0)
	sys_err("socket failed!");


	/*地址重用*/
    int on = 1;

    ret = setsockopt(udp_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(on));
    if(ret < 0)
	sys_err("setsockopt failed!");

    
    /*2.1 填写网络通信三要素的ip地址和端口号*/
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SERV_PORT);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);

    /*2.2 绑定指定服务端自己的ip地址*/
    if(bind(udp_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0){
	perror("bind failed!");
	exit(1);
    }

    struct sockaddr_in srv_addr;
    socklen_t len = sizeof(srv_addr);

    char udp_cli_ip[16];
    char buf[BUFSIZ];

    while(1)
    {
	bzero(buf, BUFSIZ);
	/*3. 接收数据*/
	ret = recvfrom(udp_fd, buf, BUFSIZ-1, 0, (struct sockaddr *)&srv_addr, &len);
	if(ret < 0)
	    continue;
	if(ret > 0)
	{
	    bzero(udp_cli_ip, 16);
	    printf("have client sent data!!!\n");
	    printf("client ip:%s，port:%d\n", inet_ntop(AF_INET, &srv_addr.sin_addr, \
			udp_cli_ip, sizeof(srv_addr)), ntohs(srv_addr.sin_port));
	    printf("pro-client said：%s\n", buf);
	}

	if(!strncasecmp(buf, QUIT_STR, strlen(QUIT_STR)))
	    //如果用户输入了quit，则退出
	    break;
    }

    close(udp_fd);
    return 0;
}
