
#include "kernellist.h"


/********1.初始化内核链表*********/
void kernellist_init(kernel_plist *head)
{
      *head = (kernel_plist)malloc(sizeof(kernel_list));
      if(NULL == *head)
      {
	    perror("*list malloc failed!");
	    exit(1);
      }
   //   memset((*head)->data,0,sizeof(初始化内核链表的数据域)

      INIT_LIST_HEAD(&(*head)->list);  //内核链表初始化函数
}

/**********2.创建链表********/
void kernellist_create(kernel_plist head)
{
      int i,n;
      kernel_plist new;

      printf("请输入需要插入的节点数目:");
      scanf("%d",&n);
      for(i = 0;i < n;i++)
      {	    //给创建的新节点申请空间
	    new = (kernel_plist)malloc(sizeof(kernel_list));
	    if(NULL == new)
	    {
		  perror("new malloc failed!");
		  exit(1);	  
	    }
	    printf("请输入数据data:");
	    scanf("%d",&new->data);     //将数据存储到链表的数据域中

	  //  kernellist_insert_tail(head,new);   //封装函数,体现模块化编程思想
	    list_add_tail(&new->list, &head->list);    //内核链表的尾插入法

            kernellist_show(head);
      }
}

//判断链表是否为空
bool is_empty_kernellist(kernel_plist head)
{
      return list_empty(&head->list);
}


           //链表的简单处理
void kernellist_sort(kernel_plist head)
{
      kernel_plist p,t;

     // p = head->prev;   //将p指针指向表尾
      p = list_entry(head->list.prev, kernel_list, list);

      while(p != head)
      {
	    if(p->data % 2 == 1)
		//  p = p->prev;
		  p = list_entry(p->list.prev,kernel_list,list);
	    else 
	    {
		  t = p;
		//  p = p->prev;  //将p指针从表尾依次向前移动
		  p = list_entry(p->list.prev, kernel_list, list);

		  //将t的节点剪切, 再插入到表尾(插入到head前面)
		  list_move_tail(&t->list, &head->list);

	    }
      }

      kernellist_show(head);
}

///遍历链表
void kernellist_show(kernel_plist head)
{
      kernel_plist p;

      printf("|_|");

      list_for_each_entry(p, &head->list, list)
      {
            printf("--->%d",p->data);
      }

      printf("\n");
}

















