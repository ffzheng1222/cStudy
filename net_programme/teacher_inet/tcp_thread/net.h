#ifndef __NET_H__
#define __NET_H__

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


struct cli_info {
    int cli_fd;
    struct sockaddr_in cin;
    pthread_t tid;
 //   list_head list;  /* 内核链表 */
};

#endif
