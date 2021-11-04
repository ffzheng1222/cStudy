

#ifndef __SELF_PRO_BACK_H_
#define __SELF_PRO_BACK_H_


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define DISH_TYPE_NUM 3   //有多少道菜，菜名个数
#define DISH_TYPE_NAME_LEN 20   //单个菜名的菜名长度(菜名数组下标)


struct menu_info_st{
    char name[DISH_TYPE_NAME_LEN];
    int price;
}menu_info[DISH_TYPE_NUM];

extern  void back_up1_min(char *j_buf, int *dish_num);
extern void back_up2_min(char *j_buf, struct menu_info_st *menu_pinfo);



#endif
