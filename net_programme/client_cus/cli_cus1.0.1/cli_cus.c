#include "cli_cus.h"

static int cli_fd = -1;

void catch_sig(int signum)
{
    int ret;
    char buf[8];

    if(signum == SIGURG)
    {
	//如果是带外数据就接收
	ret = recv(cli_fd, buf, 8, MSG_OOB);
	if(ret > 0)
	{
	    buf[ret] = '\0';
	    if(!strcmp(buf, "N"))
		printf("已结帐\n");
	    exit(0);
	}
    }
}



void show_menu(void)
{
    printf("1. 本店菜单 \n");
    printf("2. 结    账 \n");
    printf("请输入您的选择：");
} 



void err_sys(char *s)
{
    perror(s);
    exit(1);
}

/**************主函数开始****************/
int main(void)
{

    int ret = -1;
    int i = 0;

    struct info recv_info, send_info;
    struct menu_info_st menu_info[DISH_TYPE_MAX];

    struct order_dish_infomation once_dish_info[ONCE_DISH_MAX];
//    struct order_dish_infomation *once_dish_pinfo = once_dish_info; //指向结构体数组once_dish_info


    enum cmd_set CLI_SEND_INFO, CLI_ORDER_DISH, CLI_END;
    enum machine SELF_SERVER, SELF_CLI_COOK, SELF_CLI_1_CUS; 

    int all_price = 0;
    char recv_back_buf[BACK_BUFSIZE]; //服务器发送到客户端的数据包(json协议包)
    char send_back_buf[BACK_BUFSIZE]; //客户端发送到服务器的数据包(json协议包)
    const char *tmp_recv_buf = recv_back_buf;
    const char *tmp_send_buf = send_back_buf;



    //1.建立socket连接
    cli_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(cli_fd < 0)
	err_sys("socket");


    //2.1绑定服务端三要素
    struct sockaddr_in serv_peer;
    serv_peer.sin_family = AF_INET;
    serv_peer.sin_port = htons(SER_PORT);
    serv_peer.sin_addr.s_addr = inet_addr(SERV_IP);
    //2.服务器
    ret = connect(cli_fd, (struct sockaddr *)&serv_peer, sizeof(serv_peer));





    if(ret < 0)
    {
	err_sys("connect");
    }else
    {
	printf("连接成功！\n");
	// system("clear");
	//发送带外数据，让服务器知道客户端已上线
	//if(send(cli_fd, "N", 1, MSG_OOB) < 0)
	//  err_sys("send");


	//服务器发送包数据到客户端
	if((ret = read(cli_fd, recv_back_buf, BACK_BUFSIZE)) < 0)
	    err_sys("read");
	recv_back_buf[ret] = '\0';



	printf("recv_back_buf = %s\n",recv_back_buf);



	//解包，将包(json协议)数据解析到recv_info结构体中
#ifdef TEST_DEBUG
	back_up_json_max(&recv_info, &tmp_recv_buf); 
#endif
	//判断包数据是否正确
	CLI_SEND_INFO = SEND_INFO;  //取enum cmd_set枚举的成员SENG_INFO
	if(recv_info.CMD == CLI_SEND_INFO)
	{   
	    //  bzero(menu_info[].name, sizeof(menu_info[].name)); 这个要清空？
	    //解析buf中的数据   拆包：(自定义协议self_2_hotel)
#ifdef TEST_DEBUG
	    back_up2_min(recv_info.buf, menu_info);
#endif

	    while(1)
	    {
		//客户选择的选项
		int choose;
		//显示主界面
		show_menu();
		scanf("%d",&choose);
		switch(choose)
		{ 
		    case 1:
			//查看菜单，点菜
			{  
			    //临时存放一次菜单和价格
			    char tmp_buf[BACK_BUFSIZE] = "\0";
			    //存放全部
			    char send_small_buf[BACK_BUFSIZE] = "\0";
			    int order_choose, dish_num;

			    int flgs = 0;	//初始化记录顾客点菜记录
				//记录一次点菜的信息之前清空结构体成员erver_dish_price
			    bzero(once_dish_info, sizeof(once_dish_info));

			    while(1)
			    {
				//显示菜单
				for(i = 0; i < DISH_TYPE_NUM; i++)
				    printf("%d.%s ￥%d\n", i+1, menu_info[i].name, menu_info[i].price);
				printf("%d.退出点餐\n", DISH_TYPE_NUM+1);

				//客户点菜
				printf("请输入你需要点的菜的编号:");
				scanf("%d", &order_choose);


				    //记录一次点菜的每一道菜名的名称
				strncpy(once_dish_info[order_choose-1].erver_dish_name,\
					menu_info[order_choose-1].name, \
				    //记录所点菜名的菜价（单价）
					strlen(menu_info[order_choose-1].name));
				once_dish_info[order_choose-1].erver_dish_price = \
						    menu_info[order_choose-1].price;


				flgs++;	    //顾客点菜记录自曾1 

				//当客户点完菜，退出时
				if(order_choose == DISH_TYPE_NUM+1)
				{
				    //填充数据包的结构体
				    //取出枚举enum cmd_set的成员 ORDER_DISH
				    CLI_ORDER_DISH = ORDER_DISH;    
				    //取出枚举enum machine 的成员 CLI_1_CUS
				    SELF_CLI_1_CUS = CLI_1_CUS;	    
				    //取出枚举enum machine 的成员 CLI_COOK
				    SELF_CLI_COOK = CLI_COOK;	 

				    send_info.order_num = recv_info.order_num;
				    send_info.src_id = SELF_CLI_1_CUS;
				    send_info.drc_id = SELF_CLI_COOK;
				    send_info.CMD = CLI_ORDER_DISH;
				    strcpy(send_info.buf, send_small_buf);
				    //封包(json协议)
#ifdef TEST_DEBUG
printf("****5****\n");
				    back_on_json_max(send_info, &tmp_send_buf);
printf("****6****\n");
#endif
				    //发送给服务端
				    if(write(cli_fd, send_back_buf, BACK_BUFSIZE-1) < 0) 
					err_sys("write");

				    break;
				}

				else
				    //客户点几份菜
				{
				    
				    printf("请输入你所点的菜的份数:");
				    scanf("%d", &dish_num);

					//记录这道菜点了多少份
				    once_dish_info[order_choose-1].erver_dish_num = dish_num;
					//记录顾客这道菜的消费总价
				    once_dish_info[order_choose-1].the_dish_all_price = \
						menu_info[order_choose-1].price * dish_num;          

				    //计算本次消费的金额
				    all_price += menu_info[order_choose-1].price * dish_num;          
    
				    //打包（self_）
				    strncpy(tmp_buf, send_small_buf, strlen(send_small_buf));
				    bzero(send_small_buf, sizeof(send_small_buf));

				    snprintf(send_small_buf, sizeof(send_small_buf)+sizeof("2*4#"),\
					    "%s%d*%d#", tmp_buf, order_choose-1,dish_num);
				

				    for(i = 0; i < ONCE_DISH_MAX; i++)
				    {
					if(once_dish_info[i].erver_dish_price != 0)
					{					    
					    printf("您所选择的菜名：%s ￥%d,份数为(%d)\n",\
						  once_dish_info[i].erver_dish_name, \
						  once_dish_info[i].erver_dish_price,\
						  once_dish_info[i].erver_dish_num);
					}
				    }
				    printf("\t总计：消费金额￥%d\n", all_price);
				}
			    }
			    break; 
			}

		    case 2:
			{
			    //结帐操作
			    printf("本次共消费了￥%d元\n", all_price);
			    printf("请等待服务员来结帐\n");

			    //填充数据包的结构体
			    CLI_END = END;  //取得枚举enum cmd_set中的END成员
			    SELF_SERVER = SERVER;    //取得enum machine中的SERVER成员
			    send_info.order_num = recv_info.order_num;
			    send_info.src_id = SELF_CLI_1_CUS;
			    send_info.drc_id = SERVER;
			    send_info.CMD = CLI_END;

#ifdef TEST_DEBUG
			    /*按照（自定义协议self_1_hotel）的格式严格封包*/
			    //寻找self_1_hotel协议中的“*”分格符在buf中的对应位置
			    char *p1_buf = &send_info.buf[2];	
			    //寻找self_1_hotel协议中的“#”分格符在buf中的对应位置
			    char *p2_buf = &send_info.buf[4];
			    //把send_info.buf 内的元素都（初始化）为'x'
			    memset(send_info.buf, 'x', sizeof(send_info.buf));

			    //指向send_buf的两个指针移动，判断到达末尾的边界条件
			    for(i = 0; (!strncmp(p1_buf, "x", 1))&&(!strncmp(p2_buf, "x", 1)); i++)
			    {
				*p1_buf = '*';
				*p2_buf = '#';

				p1_buf = p1_buf + 4;    //指针每次移动4个位置
				p2_buf = p2_buf + 4;    //指针每次移动4个位置
			    }

#ifdef TEST_DEBUG
			    //封json协议包
			    back_on_json_max(send_info, &tmp_send_buf);
#endif

#endif
			    //发送给服务端
			    if(write(cli_fd, send_back_buf, BACK_BUFSIZE-1) < 0)
				err_sys("write");

			    //暂停，等待带外数据的到来
			    pause();

			    //信号注册
			    signal(SIGURG, catch_sig);
			    //设置属主进程
			    fcntl(cli_fd, F_SETOWN, getpid());
			    break;

			}

		    default:
			printf("\n请输入 1.显示菜单 2.结账\n");
			break;

		}
	    }

	}
    }
    return 0;
}    
