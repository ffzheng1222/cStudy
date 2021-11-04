#ifndef  __NET_H_
#define  __NET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
 
 #include <arpa/inet.h>
 
 /*通过man 7 ip 得到的头文件*/
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netinet/ip.h>

 
 #define SERV_IP   "192.168.7.89"    //服务器的ip地址
 #define SERV_PORT 5002      //服务器的端口号
 #define QUIT_STR  "quit"


struct cli_info{
    int cli_fd;
    struct sockaddr_in cin; 
    pthread_t tid;
//    list_hand list; 
};

#endif
