
#include "doublelist.h"


//初始化双向循环链表
void doublelist_init(double_plist *head)
{
      *head = (double_plist)malloc(sizeof(double_list));
      if(NULL == *head)
      {
	    perror("*list malloc failed!");
	    exit(1);
      }
   //   memset((*head)->data,0,sizeof(初始化双向循环链表的数据域

      (*head)->next = *head;   //初始化双向循环链表的指针域
      (*head)->prev = *head;
}

//创建链表
void doublelist_create(double_plist head)
{
      int i,n;
      double_plist new;

      printf("请输入需要插入的节点数目:");
      scanf("%d",&n);
      for(i = 0;i < n;i++)
      {	    //给创建的新节点申请空间
	    new = (double_plist)malloc(sizeof(double_list));
	    if(NULL == new)
	    {
		  perror("new malloc failed!");
		  exit(1);	  
	    }
	    printf("请输入数据data:");
	    scanf("%d",&new->data);     //将数据存储到链表的数据域中

	    doublelist_insert_tail(head,new);   //封装函数,体现模块化编程思想

            doublelist_show(head);
      }
}

              /*双向循环链表的插入操作*/
//向head后面插入节点/向表头插
void doublelist_insert(double_plist head,double_plist new)
{
      new->next = head->next;
      head->next->prev = new;
      new->prev = head;
      head->next = new;
}
//向head前面插入节点/向表尾插
void doublelist_insert_tail(double_plist head,double_plist new)
{
      new->prev = head->prev;
      head->prev->next = new;
      new->next = head;
      head->prev = new;
}

	      /*双向循环链表的删除操作*/
//删除p指向的节点
void doublelist_del(double_plist p)
{
      p->prev->next = p->next;
      p->next->prev = p->prev;
}
//删除p前面的节点
void doublelist_del_before(double_plist p)
{
      doublelist_del(p->prev);
}
//删除p后面的节点
void doublelist_del_after(double_plist p)
{
      doublelist_del(p->next);
}

//判断链表是否为空
bool is_empty_doublelist(double_plist head)
{
      if(head == head->next)
	    return true;
      else
	    return false;
}

//链表的简单处理
void doublelist_sort(double_plist head)
{
      double_plist p,t;

      p = head->prev;   //将p指针指向表尾

      while(p != head)
      {
	    if(p->data % 2 == 1)
		  p = p->prev;
	    else 
	    {
		  t = p;
		  p = p->prev;  //将p指针从表尾依次向前移动

		  //将t的节点剪切, 再插入到表尾(插入到head前面)
		  doublelist_del(t);		  
		  doublelist_insert_tail(head,t);
	//	  doublelist_show(head);
	    }
      }

      doublelist_show(head);
}

///遍历链表
void doublelist_show(double_plist head)
{
      double_plist p;

      for(p = head->next ; p != head;p = p->next)
	    printf("%d\t",p->data);

      printf("\n");
}

