#ifndef _CLI_CUS_H_
#define _CLI_CUS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <signal.h>
#include <fcntl.h>
#include </usr/local/include/json/json.h>


#include "json_back.h"
#include "self_pro_back.h"



#define SERV_IP  "192.168.7.89"
#define SER_PORT  22222


//接受/发送（json包）的临时 buf
#define BACK_BUFSIZE 128



//菜单上可供顾客选择的菜的种类上限
#define DISH_TYPE_MAX 128
//顾客一次最多可以选择的菜的种类
#define ONCE_DISH_MAX 64




// 顾客选择每一种菜名的信息
struct order_dish_infomation{
    char erver_dish_name[DISH_TYPE_NAME_LEN];  //所选择的菜名
    int erver_dish_price;   //所选菜名的价格
    int erver_dish_num;	    //选择的这种菜名的份数
    int the_dish_all_price;   //选择的这种菜名的总价
};

enum cmd_set{
    SEND_INFO = 10,
    ORDER_DISH,
    COOKING ,
    SER_DISH,
    END
};


enum machine{
    SERVER = 1,
    CLI_COOK  ,
    CLI_1_CUS ,
    CLI_2_CUS ,
    CLI_3_CUS 
};


extern void catch_sig(int signum);
extern void err_sys(char *s);
extern void show_menu();



#endif
