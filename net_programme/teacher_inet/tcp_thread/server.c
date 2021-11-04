#include "net.h"
/*./server [serv_port]*/
#include <pthread.h>
int cli_data_handle(void *arg);

void usage(char *s)
{

    printf("Usage:\n\t%s[serv_port]\n",s);
    printf("\tserv_port: the server port that you want to set(serv_port > 5000).\n");
    printf("\n");
}


int main(int argc, char *argv[])
{
    int fd = -1;
    struct sockaddr_in sin;
    int port = SERV_PORT;

   if(argc!=1 && argc != 2) {
        usage(argv[0]);
        exit(1);
    }
     if(argc == 2) {
        port = atoi(argv[1]);
        if( (port < 5000) && (port <= 65535)) {
            usage(argv[0]);
            exit(1);
        }
    }

    /* 1.创建套接字 */
    fd = socket(AF_INET, SOCK_STREAM, 0); //建立IPV4 TCP通信
    if(fd < 0) {
	perror("socket");
	exit(1);
    } 
    
    /*优化3： 改变套接字的属性为： 允许地址快速重用 */
    int reuse = 1;
    setsockopt(fd, SOL_SOCKET,SO_REUSEADDR, &reuse, sizeof(int));


    /*2.1 填写网络通信三要素的IP地址和端口号 */
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port =  htons(port);
#if 0
    sin.sin_addr.s_addr = inet_addr(SERV_IP);
#else 
    sin.sin_addr.s_addr = htonl(INADDR_ANY); /*优化1： 绑定在本地的任意IP地址 */
#endif   
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
    struct sockaddr_in cin;
    socklen_t addrlen = sizeof(cin);

    /*4.阻塞等待客户端的连接 */
  pthread_t tid;
  
  while(1) {
      
       newfd = accept(fd, (struct sockaddr *)&cin, &addrlen);
       if(newfd < 0) {
          perror("accept");
          exit(1);
       }
       struct cli_info *p_cli_info = (struct cli_info *)malloc(sizeof(struct cli_info));      
       if(!p_cli_info) continue;
       
       p_cli_info->cli_fd = newfd;
       memcpy(&p_cli_info->cin, &cin, sizeof(cin));
       memcpy(&p_cli_info->tid, &tid, sizeof(tid));
	
       pthread_create(&tid, NULL, (void *)cli_data_handle, (void *)p_cli_info);
       pthread_detach(tid); //设置分离线程
    }
    close(fd);
    return 0;
}

int cli_data_handle(void *arg)
{
    int newfd;
    struct sockaddr_in cin;
    char  cli_ip_addr[16];
    struct cli_info *p_cli_info = (struct cli_info *)arg;
    
    if(!p_cli_info) return -1;
     
    newfd = p_cli_info->cli_fd;
    memcpy(&cin, &p_cli_info->cin, sizeof(cin));
    
      if(inet_ntop(AF_INET,(void *)&cin.sin_addr.s_addr, cli_ip_addr, sizeof(cin)) ) {
           printf("Client(%s:%d) is connected!\n",cli_ip_addr, ntohs(cin.sin_port));
   }

    int ret = -1;
    char buf[BUFSIZ];
    while(1) {
         bzero(buf, BUFSIZ);
         do {
            ret = read(newfd,buf, BUFSIZ-1);
         }while(ret < 0 && EINTR == errno);

         //处理收到的数据
         //...
         if(ret != 0 )printf("Client(%s:%d) said:%s\n", cli_ip_addr, ntohs(cin.sin_port), buf);

         if(!ret || !strncasecmp(buf, QUIT_STR, strlen(QUIT_STR))) { //如果客户端退出或发送quit
               printf("Client(%s:%d) is exited\n", cli_ip_addr, ntohs(cin.sin_port));
               break;
         }
    }

    close(newfd);
    return 0;	
}
    
