#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

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
