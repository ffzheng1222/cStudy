

#include "self_pro_back.h"


//拆自定义协议self_2_hotel的函数处理
void back_up2_min(char *j_buf, struct menu_info_st *menu_pinfo)
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



