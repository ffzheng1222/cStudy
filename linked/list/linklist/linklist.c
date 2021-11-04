#include "linklist.h"

//初始化链表
void linklist_init(link_plist *head)
{
      *head = (link_plist)malloc(sizeof(link_list));
      if(NULL == *head)
      {
	    perror("*list malloc failed!");
	    exit(1);
      }
   //   memset((*head)->data,0,sizeof(datatype));  //初始化链表的数据域

      (*head)->next = NULL;   //初始化链表的指针域
}

//创建链表
void linklist_create(link_plist head)
{
      int i,n;
      link_plist p,new;

      p = head;  //p代替头指针操作链表

      printf("请输入需要插入的节点数目:");
      scanf("%d",&n);
      for(i = 0;i < n;i++)
      {	    //给创建的新节点申请空间
	    new = (link_plist)malloc(sizeof(link_list));
	    if(NULL == new)
	    {
		  perror("new malloc failed!");
		  exit(1);	  
	    }
	    printf("请输入数据data:");
	    scanf("%d",&new->data);    //将数据存储到链表的数据域中
#if 0 
	    new->next = p->next;       //方法一
	    p->next = new;
#else
	    linklist_insert(p,new);    //方法二
				       //封装函数,体现模块化编程思想
#endif
	    p = p->next;

            linklist_show(head);
      }
}

//插入链表
void linklist_insert(link_plist p,link_plist new)
{
      new->next = p->next;
      p->next = new;
}

#if 0
//删除链表指定的位置
bool linklist_del(link_plist head,int i)
{
      int j;
      link_plist t,p;

      p = head;   //p指针代替头指针操作链表

      for(j = 0;j < i && p != NULL;j++)
	    p = p->next;
      if(NULL == p->next)
      {
	    printf("待删除的节点不存在!");
	    exit(1);
      }

      t = p->next;    
      p->next = t->next;

      free(t);    //释放已删除的节点
      t = NULL;

      return true; 
}

#else

//删除链表
bool linklist_del(link_plist head)
{
      link_plist t,p;

      p = head;   //p指针代替头指针操作链表

      if(NULL == p->next)
      {
	    printf("待删除的节点不存在!");
	    exit(1);
      }

      t = p->next;    
      p->next = t->next;

      free(t);    //释放已删除的节点
      t = NULL;

      return true; 
}
#endif

//判断链表是否为空
bool is_empty_linklist(link_plist head)
{
      link_plist p;

      p = head;     //p代替头指针操作链表

      if(NULL == p->next)
	    return true;
      else
	    return false;
}
//链表的逆置
void linklist_sort(link_plist head)
{
      link_plist p,t;

      //将原链表分为两个链表,一个空链表,一个数控链表
      p = head->next;
      head ->next = NULL;

      while(NULL != p)
      {
	    t = p;   //将p指向的节点地址保存到t变量
	    p = p->next;
#if 0
	    t->next = head->next;	    
	    head->next = t;
#else
	    linklist_insert(head,t);
#endif
      } 
	    linklist_show(head);
}

///遍历链表
void linklist_show(link_plist head)
{
      link_plist p;

      for(p = head->next ; p != NULL;p = p->next)
	    printf("%d\t",p->data);

      printf("\n");
}

