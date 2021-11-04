/* client.c */
/*
    usage:
	./client  [serve_ip]  [serve_port] 
*/

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


void usage(char *s)
{
    printf("usage: \n\t%s  [serv_ip]  [serve_port]\n", s);
    printf("\t[serv_ip]:the serv ip that you want to conncet!\n");    
    printf("\t[serve_port]:the serve port that you want to conncet!(serve port>5000)\n");    
}

int main(int argc, char *argv[])
{
    int tcp_fd = -1;
    int port = SERV_PORT;
    char serv_ip_addr[16];

    if(argc != 1 && argc != 3){
	usage(argv[0]);
	exit(1);
    }

    bzero(serv_ip_addr, 16);
    strncpy(serv_ip_addr, SERV_IP, strlen(SERV_IP));

    if(argc == 3)
    {
	port = atoi(argv[2]);
	if(port < 5000 || port >= 65535){
	    usage(argv[0]);
	    exit(1);
	}

	//判断用户输入的ip地址是否合法
	//...FIXME!!!

	bzero(serv_ip_addr, 16);
	int len = strlen(argv[1]);
	if(len > 15) len = 15;
	strncpy(serv_ip_addr, argv[1], len);
    }

    /*1. 创建套接字*/
    tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(tcp_fd < 0){
	perror("socket failed!");
	exit(1);
    }

    /*2.1 填写网络通信三要素的ip地址和端口号*/
    struct sockaddr_in sin;
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(serv_ip_addr);

    /*2.2 链接的是服务器的端口*/
    if(connect(tcp_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0){
	perror("conncet failed!");
	exit(1);
    }

    char buf[BUFSIZ];
    while(1)
    {
	bzero(buf ,BUFSIZ);
	fprintf(stderr,"please input string：");
	if(fgets(buf ,BUFSIZ-1 ,stdin) == NULL){
	    perror("fgets failed!");
	    continue;
	}
    
	//写入套接字
	write(tcp_fd ,buf ,BUFSIZ-1);

	if(!strncasecmp(buf ,QUIT_STR ,strlen(QUIT_STR)))  //用户输入quit退出
	    break;
    }

    close(tcp_fd);
    return 0;
}
