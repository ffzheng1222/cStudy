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

#define SERV_IP "192.168.7.4"
#define SERV_PORT 5001
#define QUIT_STR "quit"

int main(void)
{
    int fd = -1;
    struct sockaddr_in sin;

    /* 1.创建套接字 */
    fd = socket(AF_INET, SOCK_STREAM, 0); //建立IPV4 TCP通信
    if(fd < 0) {
	perror("socket");
	exit(1);
    } 

    /*2.1 填写网络通信三要素的IP地址和端口号 */
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port =  htons(SERV_PORT);
    sin.sin_addr.s_addr = inet_addr(SERV_IP);
    /*2.2 绑定 */
    if( bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
	perror("bind");
	exit(1);
    }   

    /*3.把主动套接字变成被动套接字 */
    if(listen(fd, 5)< 0) {
        perror("listen");
        exit(1);
    }
    printf("Server started!\n");    
    int newfd = -1; 
    /*4.阻塞等待客户端的连接 */
    newfd = accept(fd,NULL, NULL);
    
    int ret = -1;
    char buf[BUFSIZ];
    while(1) {
	 bzero(buf, BUFSIZ);
	 do {
	    ret = read(newfd,buf, BUFSIZ-1);
	 }while(ret < 0 && EINTR == errno);
	
	 //处理收到的数据 
	 //...
	 printf("Client(fd=%d) said:%s\n", newfd, buf);

	 if(!strncasecmp(buf, QUIT_STR, strlen(QUIT_STR))) { //如果客户端发送quit
	       close(newfd);	
	       break;
	 }
    }

    close(fd);
    return 0;
}
