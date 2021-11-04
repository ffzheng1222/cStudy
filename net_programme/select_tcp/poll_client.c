/* client.c */
/*
usage:
./client  [serve_ip]  [serve_port] 
 */

#include "net.h"
#include <poll.h>

void usage(char *s)
{
    printf("usage: \n\t%s  [serve_ip]  [serve_port]\n", s);
    printf("\t[serve_ip]:the serve ip that you want to conncet!\n");    
    printf("\t[serve_port]:the serve port that you want to conncet!(serve port>5000)\n");    
}

int main(int argc, char *argv[])
{
    int tcp_fd = -1;
    int ret = -1;

    struct sockaddr_in sin;
    int port = SERV_PORT;
    char serv_ip_addr[16];

    if(argc != 1 && argc != 3)
    {
	usage(argv[0]);
	exit(1);
    }

    bzero(serv_ip_addr, 16);
    strncpy(serv_ip_addr, SERV_IP, strlen(SERV_IP));

    if(argc == 3)
    {
	port = atoi(argv[2]);
	if(port < 5000 || port >= 65535)
	{
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
    if(tcp_fd < 0)
    {
	perror("socket failed!");
	exit(1);
    }

    /*2.1 填写网络通信三要素的ip地址和端口号*/
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(serv_ip_addr);

    /*2.2 链接的是服务器的ip地址*/
    if(connect(tcp_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
	perror("conncet failed!");
	exit(1);
    }

    char buf[BUFSIZ];
    char kbuf[BUFSIZ];

    struct pollfd fds[2];
    fds[0].fd = tcp_fd;
    fds[0].events = POLLIN;

    fds[1].fd = 0;
    fds[1].events = POLLIN;

    while(1)
    {
	ret = poll(fds, 2, -1);
	if(ret < 0)
	{
	    perror("poll failed!");
	    exit(1);
	}
	if(ret > 0)
	{
	    if(fds[0].revents & POLLIN)
	    {
		bzero(buf, BUFSIZ);
		ret = read(tcp_fd, buf, BUFSIZ-1);
		if(ret < 0)
		{
		    perror("read failded!");
		    exit(1);
		}
		if(ret == 0)
		{
		    printf("poll server disconnect...!\n");
		    exit(1);
		}
		if(ret > 0)
		    printf("server to poll client said：%s\n", buf);
	    }

	    if(fds[1].revents & POLLIN)
	    {
		bzero(kbuf, BUFSIZ);
		if(fgets(kbuf, BUFSIZ-1, stdin) == NULL)
		{
		    perror("fgets failed!");
		    exit(1);
		}
		printf("poll client said：%s\n", kbuf);
		write(tcp_fd, kbuf, strlen(kbuf));
		if(!strncasecmp(kbuf ,QUIT_STR ,strlen(QUIT_STR)))
		    break;
	    }

	}
    }




#if 0 
    /*3. 通过select监听：标准输入，tcp_fd */
    int max_fd = tcp_fd;
    fd_set readset;

    while(1)
    {
	FD_ZERO(&readset);
	//将需要监听的fd加入到集合中
	FD_SET(0, &readset);
	FD_SET(tcp_fd, &readset);

	ret = select(max_fd+1, &readset, NULL, NULL, NULL);
	//表示select函数出错
	if(ret < 0)
	{   
	    perror("select failed!");
	    exit(1);
	}
	//表示谁产生了数据(哪一个fd来了数据)
	if(ret > 0)
	{   
	    //判断服务器是否有数据？？？？？
	    if(FD_ISSET(tcp_fd, &readset))
	    {
		bzero(buf, BUFSIZ);
		ret = read(tcp_fd, buf, BUFSIZ);
		if(ret < 0)
		{
		    perror("read failed!");
		    exit(1);
		}

		printf("client recived server to saided：%s\n", buf);	

		if(ret == 0)
		{
		    printf("server disconnect!\n");
		    tcp_fd = -1;
		    exit(1);
		}

	    }
	    //判断服务器的标准输入是否有数据
	    if(FD_ISSET(0, &readset))
	    {	
#if 1
		bzero(kbuf ,BUFSIZ);

		//buf未写完，一直阻塞等待写，怎么解决
		//???????????????????????????????????????
	//	fprintf(stdout,"client to server saided：\n");
		if(fgets(kbuf ,BUFSIZ-1 ,stdin) == NULL)
		{
		    perror("fgets failed!");
		    continue;
		}
		//????????????????????????????????????????

		//写入套接字
		write(tcp_fd ,kbuf ,strlen(kbuf));
#else
		write(tcp_fd, s, strlen(s));
		sleep(3);
#endif
		if(!strncasecmp(kbuf ,QUIT_STR ,strlen(QUIT_STR)))  //用户输入quit退出
		    break;
	    }
	}
    }
#endif
    close(tcp_fd);
    return 0;
}
