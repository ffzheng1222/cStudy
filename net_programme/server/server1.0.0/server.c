

#include "server.h"

#define SERV_ID "192.168.7.89"
#define SERV_PORT 22222

#define COOKING_IP "192.168.7.74"

#define ORDER_DISH 16
#define SERVER 1
#define CLI_COOK 2
#define END 17
#define SER_DISH 18
#define COOKING 14
#define SEND_INFO 19
#define DISH_TYPE_NAME_LEN 128

//带外数据的处理


#define DESK_NUM 3//桌子总数
#define DISH_TYPE_NUM 3//菜品总类
#define LOGIN "N"
#define LOGOFF "F"




struct on_dish
{
      //套接字
      int cli_fd;
      //订单号
      int order_num;
      //自己的源id
      int src_id;
      //消费记录
      char buf[128];
};


struct info{
      int src_id;
      int drc_id;
      int order_num;
      int CMD;
      char buf[BUFSIZ];
};



//先用数组代替链表，里面的成员对应数据库
struct dish_information{
      int dish_num;//编号
      int price;//价格
      char name[128];//名字
};





void back_on_json_max(struct info buf_info, const char **my_str);

void back_up_json_max(struct info *buf_info, const char ** my_str);

void back_up1_min(char *j_buf, int *dish_num);

void back_up2_min(char *j_buf, struct dish_information *menu_pinfo);

void err_sys(char *s)
{
      perror(s);
      // exit(1);	
}

int main(void)
{
      //启动服务器
      int server_fd = -1;
      int i,j;
      int ret = -1;
      struct sockaddr_in serv_sin,cli_cus;
      char cli_desk_num[3][16] = {"192.168.7.85","192.168.7.222","192.168.7.87"};
      char addr_tmp[16];
      struct on_dish cus_consume[20];//记录消费记录
      struct dish_information dish_inform[DISH_TYPE_NUM];//存放的是菜品信息
      static int order_num = 0;//订单号，是唯一主键，可以唯一确定对应i的订单
      int tmp_src_id;
#if 0
      //菜品信息处理
      bzero(dish_inform,sizeof(dis_inform));
#endif //需要sql支持，
      /*
       *.....FIXME
       *
       *
       */

      server_fd = socket(AF_INET, SOCK_STREAM, 0);
      if(server_fd < 0)
	    err_sys("socket");

#if 1      //地址重用
      int on = 1;
      ret = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(on));
      if(ret < 0)
	    err_sys("setsockopt : USRADDR");
#endif
      int oob_on = 1;
      ret = setsockopt(server_fd,SOL_SOCKET, SO_OOBINLINE,&oob_on,sizeof(oob_on));
      if(ret < 0)
	    err_sys("setsockopt:oob");
      bzero(&serv_sin, sizeof(serv_sin));
      serv_sin.sin_family = AF_INET;
      serv_sin.sin_port = htons(SERV_PORT);
      serv_sin.sin_addr.s_addr = htonl(INADDR_ANY);



      ret = bind(server_fd, (struct sockaddr *)&serv_sin, sizeof(serv_sin));
      if(ret < 0)
	    err_sys("bind");

      ret = listen(server_fd, 5);
      if(ret < 0)
	    err_sys("listen");


      printf("server start!\n");


      int max_fd = server_fd;
      fd_set r_set,err_set;//一个读集合和一个错误集合，读集合解决write问题，错误集合解决带外数据
      //设置为多个客户端模式
      //用数组把客户端的套接字存起来
      int cli_cus_fd[DESK_NUM];
      for(i = 0; i < DESK_NUM ; i++)
	    cli_cus_fd[i] = -1;

      //设置通信套接字的属性
      //设置成可以接收带外数据
      FD_ZERO(&r_set);//清空
      FD_ZERO(&err_set);
      socklen_t cli_addrlen = sizeof(cli_cus);
      int all_price;
      int tmp_fd;//中间过度载体
      int cook_fd = -1;
      char recv_back_buf[BUFSIZ];//接受命令包
      char send_back_buf[BUFSIZ];//发送命令包
      struct info recv_info,send_info;
      const char *tmp_transform_buf;
      int dish_type[DISH_TYPE_NUM];
      int dish_fin,dish_fin_num;
      while(1)
      {	    
	    //添加到集合
	    FD_SET(server_fd,&r_set);
	    FD_SET(cook_fd,&r_set);
	    //判断顾客客户端文件描述符是否合法
	    for(i = 0; i < DESK_NUM ; i++)
	    {
		  if(cli_cus_fd[i] > 0)
		  {
			FD_SET(cli_cus_fd[i],&r_set);
			FD_SET(cli_cus_fd[i],&err_set);
		  }
	    }
	    //调用select函数监听
	    ret  = select(max_fd+1 , &r_set , NULL, &err_set, NULL);
	    if(ret < 0)
	    {
		  err_sys("select");
		  //出错判断
	    }else
	    {
		  //监听动作
		  //1.监听accepct
		  if(FD_ISSET(server_fd,&r_set))
		  {
			bzero(&addr_tmp,sizeof(addr_tmp));
			//和客户端连接
			tmp_fd = accept(server_fd,(struct sockaddr *)&cli_cus,&cli_addrlen);
			//字节序的装换
			if(!inet_ntop(AF_INET,(void *)&cli_cus.sin_addr.s_addr,addr_tmp,sizeof(addr_tmp)) )
			      err_sys("inet_ntop");
			//判断连接的是否为厨房
			if(!strncmp(addr_tmp,COOKING_IP,sizeof(COOKING_IP)))
			{
			      //厨房客户端处理
			      cook_fd = tmp_fd;
			      max_fd = cook_fd;//更新文件描述符
			}
			else
			{//顾客端处理

			      //替代当前无效的文件描述符
			      //替代当前无效的文件描述符的数组成员
			      for(i = 0; i < DESK_NUM ; i++)
			      {
				    if(cli_cus_fd[i] == -1)
				    {
					  cli_cus_fd[i] = tmp_fd;
					  max_fd = cli_cus_fd[i];//更新文件描述符
					  break;
				    }
			      }
			      //打印一条欢迎语句
			      //确定连接的客户端的IP，从而辨别是那一个客户端连接的
			      //利用IP的唯一性确定桌子号
			      for(i = 0 ; i < DESK_NUM ; i++)
			      {
				    if(!strncmp(cli_desk_num[i],addr_tmp,sizeof(addr_tmp)))
					  tmp_src_id = i + 1;
			      }

			      order_num++;
			      printf("第%d号桌子,生成的订单号是：%d\n",tmp_src_id,order_num);

			      //把客户信息记录下来
			      //1.订单号
			      cus_consume[order_num - 1].order_num = order_num;
			      //2。桌子号
			      cus_consume[order_num - 1].src_id = tmp_src_id;
			      //3.对应的文件描述符
			      cus_consume[order_num - 1].cli_fd = tmp_fd;

#if 1
						      //1.打包成命令包CMD = COOKING;
						      //2.填充好结构体
						      bzero(&send_info,sizeof(send_info));
						      send_info.CMD = 10;
						      send_info.src_id = SERVER;
						      send_info.drc_id = CLI_COOK;
						      send_info.order_num = 64;
						      //strncpy(send_info.buf,"sad*2#qa*3#aa*3#",sizeof("sad*2#ad*3#wa*3#"));
						      //3.封包
	    snprintf(send_info.buf,3*50,"%s*%d#%s*%d#%s*%d#","鸡蛋",6,"青菜",2,"牛肉",4);
						      //bzero(tmp_transform_buf,sizeof(tmp_transform_buf)); 
						      //send_info.buf[len] = '\0';
						      //tmp_transform_buf = NULL;
						      //send_info.buf
						      printf("----------\n");
#if 1
						      back_on_json_max(send_info,&tmp_transform_buf);
#endif 

						      printf(" send.tmp_transform_buf = %s\n",tmp_transform_buf);
						      //4.发包
						      ret = write(tmp_fd,tmp_transform_buf,strlen(tmp_transform_buf));
						      if(ret < 0)
							    perror("write send_info");
#endif







			}

		  }
		  //2.监听客户是否发命令包过来
		  for(i = 0;i < DESK_NUM ;i++)
		  {
			if(cli_cus_fd[i] > 0)
			{
			      if(FD_ISSET(cli_cus_fd[i],&r_set))
			      {
				    printf("it is not oob\n");
				    ret = read(cli_cus_fd[i],recv_back_buf,BUFSIZ);

				    if(ret < 0)
				    {
					  
					  err_sys("read");
				    }else if(ret == 0){
					  printf("nothing recv\n");
				    }else{
					  //客户端发送命令过来了
					  /*
					   *拆包判断命令类型

					   *
					   */
					   recv_back_buf[ret] = '\0';
					  printf("get smothng\n");
					  printf("%s\n",recv_back_buf);
					  bzero(&recv_info,sizeof(recv_info));
					  //strncpy(tmp_transform_buf,recv_back_buf,sizeof(recv_back_buf));
					  tmp_transform_buf = recv_back_buf;
					  back_up_json_max(&recv_info,&tmp_transform_buf);
					  printf("tmp = %s\n",tmp_transform_buf);
					  //
					  switch(recv_info.CMD)
					  {
						//接收下单命令
						case ORDER_DISH:
						      //转发给厨房
						      //1.打包成命令包CMD = COOKING;
						      //2.填充好结构体
						      send_info.CMD = COOKING;
						      send_info.src_id = SERVER;
						      send_info.drc_id = CLI_COOK;
						      send_info.order_num = recv_info.order_num;
						      strncpy(send_info.buf,recv_info.buf,sizeof(recv_info.buf));
						      //3.封包
						      //bzero(tmp_transform_buf,sizeof(tmp_transform_buf));
#if 1
						      //const char * test_buf;
						      back_on_json_max(send_info,&tmp_transform_buf);
#endif 

						      printf(" recv.tmp_transform_buf = %s\n",tmp_transform_buf);
						      //4.发包
						      ret = write(cook_fd,tmp_transform_buf,strlen(tmp_transform_buf));
						      if(ret < 0)
							    err_sys("wrte CMD = COOKING");
						      recv_info.CMD = -1;
						      break;
						      //接收结帐命令
						case END:
						      /*
						       *..FIXME
						       */
						      //*********************************
						      //需要sql支持，
						      back_up1_min(recv_info.buf,dish_type);
						      all_price = 0;
						      printf("第%d号要求结账，订单号为：%d,消费如下：\n",recv_info.src_id,recv_info.order_num);
						      for(j = 0; j < DISH_TYPE_NUM ; j++)
						      {
							    if( dish_type[j] != 0)
							    {
								  printf("%s\t%d份\n",dish_inform[j].name,dish_type[j]);
								  all_price += dish_inform[j].price * dish_type[j];
							    }
						      }
						      printf("总消费为：%d\n",all_price);
						      //*****************************************************
						      //模拟服务员结账
						      sleep(1);
						      //关闭
						      //发送带外数据“F”
						      ret = send(cli_cus_fd[i], LOGOFF, 1, MSG_OOB);
						      if(ret < 0)
							    err_sys("send");
						      close(cli_cus_fd[i]);

						      recv_info.CMD = -1;
						      break;
						default:
						      printf("error CMD = %d\n",recv_info.CMD);
					  }
				    }
			      }else if(FD_ISSET(cli_cus_fd[i],&err_set))
			      {
				    printf("it is oob\n");
				    //处理带外数据
				    ret = -1;
				    ret = recv(cli_cus_fd[i], recv_back_buf, 1, MSG_OOB );//MSG_OOB标志  
				    if(ret > 0)
				    {
					  recv_back_buf[ret] = '\0';
					  printf("this is err_set\n");
					  if(!strcmp(recv_back_buf,LOGIN))
					  {
						printf("recv a oob data:%s\n",recv_back_buf);
						//打包成命令包 CMD = SEND_INFO
						//填充好结构体
						send_info.CMD = SEND_INFO;
						send_info.src_id = SERVER;
						//填充send_info.drc_id
						//由文件描述符去找消费记录的数组下标
						for(j = 0;j < 20;j++ )
						{
						      if(cus_consume[j].cli_fd == cli_cus_fd[i])
						      {	
							    send_info.drc_id = cus_consume[j].src_id;
							    send_info.order_num = cus_consume[j].order_num;
							    printf("before j = %d\n",j);
							    break;
						      }

						}

						//把菜单封包起来
						bzero(send_info.buf,sizeof(send_info.buf));
						for(j = 0; j < DISH_TYPE_NUM; j++)
						      snprintf(send_info.buf,26,"%s*%d#",dish_inform[j].name,1);
						//strncpy(send_info.buf,recv_info.buf,sizeof(recv_info.buf));
						//封包
						//bzero(tmp_transform_buf,sizeof(tmp_transform_buf));
						back_on_json_max(send_info,&tmp_transform_buf);
						ret = write(cli_cus_fd[i],tmp_transform_buf,sizeof(tmp_transform_buf));
						if(ret < 0)
						      err_sys("write : CMD = SEND_INFO");

					  }else if(ret == 0){
						printf("ret = 0\n");
					  }else
					  {//不是带外数据
						printf("recv is not a oob data\n");
					  }

				    }


			      }
			}else{
			      printf("cli_cus_fd = %d\n",cli_cus_fd[i]);
			}
		  }











		  //3.厨房端发来上菜命令
		  if(FD_ISSET(cook_fd,&r_set))
		  {				   
			ret = read(cli_cus_fd[i],recv_back_buf,BUFSIZ);
			recv_back_buf[ret] = '\0';
			//厨房端处理
			//拆包
			bzero(&recv_info,sizeof(recv_info));
			bzero(&tmp_transform_buf,sizeof(tmp_transform_buf));
			back_up_json_max(&recv_info,&tmp_transform_buf);
			//
			//判断是否为上菜命令
			if(recv_info.CMD == SER_DISH)
			{
			      //....FIXME 难点：怎么把菜名显示出来
			      sscanf(recv_info.buf,"%d*%d#",&dish_fin,&dish_fin_num);
			      printf("订单号为%d,桌子号为%d的订单中的%s*1已经做好了\n",recv_info.order_num,recv_info.drc_id,dish_inform[dish_fin].name);  
			}else{//数据包错误处理
			      printf("接受了一个错误的数据包\n");
			      continue;
			}
		  }
	    }
	    //4.监听客户端是否发送了带外数据
	    //for(i = 0;i < DESK_NUM ;i++)
	    //{

	    //}

	    sleep(2);
      }

      return 0;
}






















/*json 的打包处理*/
void back_on_json_max(struct info buf_info, const char **my_str)
{
      struct json_object *json_test, *src_id, *drc_id, *order_num, *j_buf, *cmd;
      json_test = json_object_new_object();
      src_id = json_object_new_int(buf_info.src_id);
      drc_id = json_object_new_int(buf_info.drc_id);
      order_num = json_object_new_int(buf_info.order_num);
      cmd = json_object_new_int(buf_info.CMD);
      j_buf = json_object_new_string(buf_info.buf);

      json_object_object_add(json_test, "src_id", src_id);
      json_object_object_add(json_test, "drc_id", drc_id);
      json_object_object_add(json_test, "order_num", order_num);
      json_object_object_add(json_test, "CMD", cmd);
      json_object_object_add(json_test, "buf", j_buf);
      *my_str = json_object_to_json_string(json_test);
      //*my_str = tmp_str;
      //strncpy(*my_str,tmp_str,strlen(tmp_str) + 1);
}










//拆自定义协议self_1_hotel的函数处理
void back_up1_min(char *j_buf, int *dish_num)
{
      int i = 0; 
      int dish_type_list;	    //接收字符串菜名转化的整形的菜名编号变量
      int dish_type_copis;    //接收字符串菜名份数的整形的菜名份数变量


      char *temp_buf = NULL;  
      char *p1[DISH_TYPE_NUM] = {NULL};	//拆 “#” 保存
      char *p2[DISH_TYPE_NUM] = {NULL};	//拆 “*” 保存

      char rem_back_buf[2 * DISH_TYPE_NUM];   //接收拆出的所有元素

      temp_buf = j_buf;

      for(i = 0; i < DISH_TYPE_NUM; i++)
      {
	    p1[i] = strsep(&temp_buf, "#");	//拆 “#” 号
	    p2[i] = strsep(&p1[i], "*");	//拆 “*” 号


	    dish_type_list = atoi(p2[i]);	    
	    dish_type_copis = atoi(p1[i]);

	    if(dish_type_list < 0)
	    {
		  rem_back_buf[2*i] = 0;	//拆出来的(菜名编号)rem_back_buf数组的偶数下标成员 设为0
		  rem_back_buf[2*i+1] = 0 ;	//拆出来的(菜名份数)rem_back_buf数组的奇数下标成员 设为0

	    }
	    else
	    {
		  rem_back_buf[2*i] = dish_type_list;	//拆出来的(菜名编号)rem_back_buf数组的偶数下标成员
		  rem_back_buf[2*i+1] = dish_type_copis ;	//拆出来的(菜名份数)rem_back_buf数组的奇数下标成员
	    }
      }

      for(i = 0; i < DISH_TYPE_NUM; i++)
	    *(dish_num+i)= rem_back_buf[2*i+1];

}






//#define DISH_TYPE_NUM 3   //有多少道菜，菜名个数


//拆自定义协议self_1_hotel的函数处理
void back_up2_min(char *j_buf, struct dish_information *menu_pinfo)
{
      int i = 0; 
      char dish_type_list[DISH_TYPE_NAME_LEN];	    //接收字符串菜名变量
      int dish_type_copis;    //接收字符串菜名份数的整形的菜名份价格变量


      char *temp_buf = NULL;  
      char *p1[DISH_TYPE_NUM] = {NULL};	//拆 “#” 保存
      char *p2[DISH_TYPE_NUM] = {NULL};	//拆 “*” 保存

      char  rem_back_list_buf[DISH_TYPE_NUM][DISH_TYPE_NAME_LEN] = {"\0"};   //接收拆出的菜名元素
      int rem_back_price_buf[DISH_TYPE_NUM];	//接受拆出的菜名价格元素

      temp_buf = j_buf;

      for(i = 0; i < DISH_TYPE_NUM; i++)
      {
	    p1[i] = strsep(&temp_buf, "#");	//拆 “#” 号
	    p2[i] = strsep(&p1[i], "*");	//拆 “*” 号


	    strncpy(dish_type_list, p2[i], strlen(p2[i]) + 1);	
	    dish_type_copis = atoi(p1[i]);


	    strncpy(rem_back_list_buf[i], dish_type_list, strlen(dish_type_list));	//拆出来的(菜名)rem_back_buf数组的偶数下标成员
	    rem_back_price_buf[i] = dish_type_copis ;	//拆出来的(菜名价格)rem_back_buf数组的奇数下标成员

      }

      //将数据写入struct menu_info_st结构体
      for(i = 0; i < DISH_TYPE_NUM; i++)
      {
	    strncpy(menu_pinfo[i].name, rem_back_list_buf[i], strlen(rem_back_list_buf[i]));	
	    (*(menu_pinfo+i)).price = rem_back_price_buf[i];
      }
}

















/*json 的拆包处理*/
void back_up_json_max(struct info *buf_info, const char ** my_str)
{
      json_object *jiebao_json, *jiebao_src_id, *jiebao_drc_id, *jiebao_order_num, *jiebao_j_buf, *jiebao_cmd;
      //把字符串转换成JSON对象
      jiebao_json = json_tokener_parse(*my_str);
      //通过键值获取JSON成员
      jiebao_src_id = json_object_object_get(jiebao_json,"src_id");
      jiebao_drc_id = json_object_object_get(jiebao_json,"drc_id");
      jiebao_order_num = json_object_object_get(jiebao_json,"order_num");
      jiebao_j_buf = json_object_object_get(jiebao_json,"buf");
      jiebao_cmd = json_object_object_get(jiebao_json,"CMD");
      //还原
      //int jiebao_str; 
      (*buf_info).src_id = json_object_get_int(jiebao_src_id);
      (*buf_info).drc_id = json_object_get_int(jiebao_drc_id);
      (*buf_info).order_num = json_object_get_int(jiebao_order_num);
      (*buf_info).CMD = json_object_get_int(jiebao_cmd);

      const char *test_buf = json_object_get_string(jiebao_j_buf);
      strncpy((*buf_info).buf,test_buf,sizeof(test_buf));

#ifdef TEST_DEBUG
      printf("src_id :%d\n", buf_info.src_id);
      printf("drc_id :%d\n", buf_info.drc_id);
      printf("order_num :%d\n", buf_info.order_num);
      printf("buf :%s\n", buf_info.buf);
      printf("cmd :%d\n", buf_info.CMD);
#endif

}























































