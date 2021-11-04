/* serve.c */
/*
    usage:
	./serve.c [serve_port]
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


//#define SERV_IP   "192.168.7.89"    //服务器的ip地址
#define SERV_PORT 5002	    //服务器的端口号
#define QUIT_STR  "quit"

void usage(char *s)
{
    printf("usage: \n\t%s [serve_port]\n", s);
    printf("\tserve_port: the serve port that you want to set(serve port>5000)!\n");
    printf("\n");
}

int main(int argc, char *argv[])
{
    int tcp_fd = -1;
    int port = SERV_PORT;

    if(argc > 2){
	usage(argv[0]);
	exit(1);
    }

    if(argc == 2){
	port = atoi(argv[1]);	
	if(port < 5000 || port >= 65535){
	    usage(argv[0]);
	    exit(1);
	}
    }

    /*1. 创建套接字*/
    tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(tcp_fd < 0){
	perror("socket failed!");
	exit(1);
    }

/*???????????????????????????????????????????????????????????*/

    /*优化3.  改变套接字的默认属性:(允许<服务器>地址快速重用)*/
    int reuse = -1;
    setsockopt(tcp_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

/*????????????????????????????????????????????????????????????*/


    /*2.1 填写网络通信三要素的ip地址和端口号*/
    struct sockaddr_in sin;
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
#if 0
	/*只能是指定的ip地址能链接*/
	//sin.sin_addr.s_addr = inet_addr(SERV_IP);
#else	
	/*优化1： 本地的任意ip地址都可以连接*/
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    //INADDR_ANY = -1	
#endif

    /*2.2 绑定指定服务端自己的ip地址*/
    if(bind(tcp_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0){
	perror("bind failed!");
	exit(1);
    }

    /*3. 改变套接字d的默认属性:(把主动套接字改变为被动套接字  具有监听特性)*/
    if(listen(tcp_fd, 5) < 0){
	perror("listen failed!");
	exit(1);
    }

    printf("server start!\n");
    /*4. 阻塞等待客户端链接*/
#if 0
    int newfd = -1;
    newfd = accept(tcp_fd, NULL, NULL);   //不关心接收的客户端的信息
    if(newfd < 0){
	perror("accept failed!");
	exit(1);
    }
#else
    int newfd = -1;
    struct sockaddr_in cin;
    socklen_t addrlen = sizeof(cin);
    char cli_ip_addr[16];

    bzero(cli_ip_addr, 16);
    newfd = accept(tcp_fd, (struct sockaddr *)&cin, &addrlen);   /*优化2： 取得客户端的信息*/
    if(newfd > 0 && inet_ntop(AF_INET, (void *)&cin.sin_addr.s_addr, cli_ip_addr, sizeof(cin)) )
    {
	printf("client(%s:%d) is conncet!\n", cli_ip_addr, ntohs(cin.sin_port));
    }
    if(newfd < 0){
	perror("accept failed!");
	exit(1);
    }
#endif

    int ret = -1;
    char buf[BUFSIZ];
    while(1)
    {
	bzero(buf, BUFSIZ);
	do{
	    ret = read(newfd, buf, BUFSIZ-1);
	}while(ret < 0 && EINTR == errno);

	/* 处理数据 */
	//...FIXME!!!
		
	if(ret != 0){
	    printf("client(%s:%d)  said：%s\n", cli_ip_addr, ntohs(cin.sin_port), buf);
	}

	if(!ret || !strncasecmp(buf, QUIT_STR, strlen(QUIT_STR))){
	    //如果读到文件末尾或者用户输入了quit，则退出
	    printf("client(%s:%d) is exited!\n", cli_ip_addr, ntohs(cin.sin_port));
	    close(newfd);
	    break;
	}
    }

    close(tcp_fd);
    return 0;
}
