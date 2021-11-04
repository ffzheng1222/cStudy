/*client.c */
/*
  usage: 
     ./client [serv_ip] [serv_port]
 */
#include "net.h"
void usage(char *s)
{

    printf("Usage:\n\t%s [serv_ip] [serv_port]\n",s);
    printf("\tserv_ip: the server ip address that you want to connect.\n");
    printf("\tserv_port: the server port that you want to connect(serv_port > 5000).\n");
    printf("\n");
}

//./client [serv_ip] [serv_port] 

int main(int argc, char *argv[])
{

    int fd = -1;
    struct sockaddr_in sin;

    int port = SERV_PORT;
    char serv_ip_addr[16];

    if(argc!=1 && argc != 3) {
	usage(argv[0]);
	exit(1);
    }

    bzero(serv_ip_addr, 16);
    strncpy(serv_ip_addr, SERV_IP,strlen(SERV_IP));

    if(argc == 3) {
	port = atoi(argv[2]);
	if( (port < 5000) && (port <= 65535)) {
	    usage(argv[0]);
	    exit(1);
	}
	
	//...判断用户输入的IP地址的合法性
	//...FIXME!!!
        
        bzero(serv_ip_addr, 16); 
	int len = strlen(argv[1]);
	if(len >15) len = 15;
        strncpy(serv_ip_addr, argv[1],len);
    }    

    /* 1.创建套接字 */
    fd = socket(AF_INET, SOCK_STREAM, 0); //建立IPV4 TCP通信
    if(fd < 0) {
        perror("socket");
        exit(1);
    }

  /*2.1 填写网络通信三要素的IP地址和端口号 */
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port =  htons(port);
    sin.sin_addr.s_addr = inet_addr(serv_ip_addr);
    /*2.2 绑定 */
    if( connect(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("bind");
        exit(1);
    }
    
    printf("Client started!\n");

    char buf[BUFSIZ];
    while(1) {
        /* 从标准键盘上获取数据 */
        fprintf(stderr, "please input string:");
        bzero(buf, BUFSIZ);
        if( fgets(buf, BUFSIZ-1, stdin) == NULL) {
          perror("fgets");
          continue;
        }
        /*写入套接字 */
        write(fd, buf, strlen(buf));

        if(!strncasecmp(buf, QUIT_STR, strlen(QUIT_STR))) {   //用户输入了quit
            break;
        }
      }
      close(fd);
      return 0;	    


}
