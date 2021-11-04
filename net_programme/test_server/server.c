

#include "srever.h"

#define SERV_ADDR "192.168.7.222"
#define SERV_PORT "22222"


void err_sys(char *s)
{
    perror(s);
    exit(1);	
}

int main(void)
{
    //启动服务器
    int server_fd = -1;
    int cli_cus_fd = -1;    //nwdfd作用
    int ret = -1;
    struct sockaddr_in serv_sin;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0)
	err_sys("socket");

    //地址重用
    int on = -1;
    ret = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(on));


    bzero(&serv_sin, sizeof(srv_sin));
    serv_sin.sin_family = AF_INET;
    serv_sin.sin_port = htons(SERV_PORT);
    serv_sin.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(server_fd, (struct sockaddr *)serv_sin, sizeof(serv_sin));
    if(ret < 0)
	err_sys("bind");

    ret = listen(server_fd, 5);
    if(ret < 0)
	err_sys("listen");
    

    /*带外数据的处理，多路复用的实现*/
    int max_fd = server_fd;
    fd_set set;

    FD_ZERO(&set);
    while(1)
    {	    
	FD_SET(&set, server_fd);
	FD_SET(&set, cli_cus_fd);
	
    }


    return 0;
}
