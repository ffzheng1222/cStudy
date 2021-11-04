
#ifndef __KERNELLIST_H
#define __KERNELLIST_H	

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"

typedef int datatype;
typedef struct kernellist{
      datatype data;
      struct list_head list;
}kernel_list,*kernel_plist;

struct list_head{
		struct list_head *prev;
		struct list_head *next;
}


extern void kernellist_init(kernel_plist *head);	//初始化内核链表

extern void kernellist_create(kernel_plist head);	//创建内核链表

//extern void kernellist_insert(kernel_plist head,kernel_plist new);		//内核链表的插入

//extern void kernellist_insert_tail(kernel_plist head,kernel_plist new);	//内核链表的尾插法

//extern void kernellist_del(kernel_plist head);	//内核链表的删除

//extern void kernellist_del_before(kernel_plist p);	//内核链表的删除（向前删）

//extern void kernellist_del_after(kernel_plist p);		//内核链表的删除（向后删）

extern bool is_empty_kernellist(kernel_plist head);		//判断内核链表是否为空

extern void kernellist_sort(kernel_plist head);		//内核链表的排序		

extern void kernellist_show(kernel_plist head);		//内核链表的遍历

#endif
