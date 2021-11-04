/* serve.c */
/*
usage:
./serve.c [serve_port]
 */

#include "net.h"

void usage(char *s)
{
    printf("usage: \n\t%s [serve_port]\n", s);
    printf("\tserve_port: the serve port that you want to set(serve port>5000)!\n");
    printf("\n");
}

int main(int argc, char *argv[])
{
    int tcp_fd = -1;
    int new_fd = -1;
    int max_fd = 0;
    int ret = -1;

    struct sockaddr_in sin;
    struct sockaddr_in cin;
    socklen_t len = sizeof(cin);
    int port = SERV_PORT;

    char buf[BUFSIZ];
    char kbuf[BUFSIZ];

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
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    /*优化1： 本地的任意ip地址都可以连接*/
    sin.sin_addr.s_addr = htonl(INADDR_ANY);    //INADDR_ANY = -1	

    /*2.2 绑定指定服务端自己的ip地址*/  
    if(bind(tcp_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0){
	perror("bind failed!");
	exit(1);
    }

    printf("server started!\n");

    /*3. 改变套接字d的默认属性:(把主动套接字改变为被动套接字  具有监听特性)*/
    if(listen(tcp_fd, 5) < 0){
	perror("listen failed!");
	exit(1);
    }

    /*4. 通过select监控标准输入，tcp_fd, new_fd*/
    max_fd = tcp_fd;

    fd_set readset;
    FD_ZERO(&readset);    

    while(1)
    {
	//4.1将需要监听的fd加入集合中
	FD_SET(0, &readset);
	FD_SET(tcp_fd, &readset);
	FD_SET(new_fd, &readset);

	ret = select(max_fd+1, &readset, NULL, NULL, NULL);
	//ret小于0：表示select函数出错
	if(ret < 0)
	{
	    perror("select failed!");
	    exit(1);
	}

	//ret大于0：表示监控到数据
	/*5. 判断是谁产生数据(哪一个fd来了数据)*/
	if(ret > 0) 
	{
	    //5.1. 判断是否有客服端链接
	    if(FD_ISSET(tcp_fd, &readset))
	    {
		char cli_ip_addr[16];
		bzero(cli_ip_addr, 16);

		new_fd = accept(tcp_fd, (struct sockaddr *)&cin, &len); 		
		printf("A new client connct...\n");
		printf("client：ip = %s, port = %d\n", inet_ntop(AF_INET, &cin.sin_addr.s_addr,\
			    cli_ip_addr, sizeof(cin)), ntohs(cin.sin_port));
		max_fd = new_fd;    //更新最大文件描述符
	    }
	    //5.2. 判断客服端是否有数据
	    if(FD_ISSET(new_fd, &readset))
	    {
		bzero(buf, BUFSIZ);
		ret = read(new_fd, buf, BUFSIZ);	
		if(ret < 0)	
		{
		    perror("read failed!");
		    exit(1);
		}

		printf("server revices client to said：%s\n", buf);

		if(ret == 0)
		{   
		    printf("client disconnect...\n");
		    new_fd = -1;    //表示断开
		}
	    }    
	    //5.3. 判断客服端标准输入是否有数据
	    if(FD_ISSET(0, &readset))
	    {
		bzero(kbuf, BUFSIZ);
		fgets(kbuf, BUFSIZ-1, stdin);
		if(new_fd > 0)
		    write(new_fd, kbuf, BUFSIZ-1);
		else
		    printf("kbuf：%s\n", kbuf);
	    }


	}
    }
    close(new_fd);
    return 0;
}
