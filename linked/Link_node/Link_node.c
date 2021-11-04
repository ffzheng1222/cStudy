#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct student{
      int num;
      char name[20];
      float score;       
}A;  

typedef struct Linked{
      A	    data;
      struct Linked *next;
}B;
typedef B * C;

struct Linked *create();      //带节点创建链表

void   display(C);	      //输出链表信息
void   insert(C ,C ,int);     //插入链表     
void   delete(C ,int);        //删除链表
void   freelink(C);           //销毁链表
int    get_len(C,int);        //链表的长度


int main(void)
{
      C   Head;		//头指针
      C   newNODE;	//新插入的节点指针
      int len;

      /***********初始化链表(头指针指向的节点为空)**********/

      Head  = (C)malloc(sizeof(B));
      if(NULL == Head)
      {
	    printf("Head申请空间失败!\n");
	    exit(1);
      }

      memset(&(Head->data),0,sizeof(A));  //初始化链表的数据域
      Head->next = NULL;           //初始化链表的指针域

      newNODE = (C)malloc(sizeof(B));
      if(NULL == newNODE)
      {
	    perror("newNODE malloc failed!");
	    exit(1);
      }

      Head = create();   //链表创建函数

//    display(Head);	//链表的输出函数   

      insert(Head,newNODE,2);   //链表的插入函数	     
      printf("插入后的链表:\n");
      display(Head);
      /*插入后链表的长度*/
      len = get_len(Head,len);   //链表的长度(大小)函数
      printf("此链表的长度为:\nlen = %d\n",len);     

      delete(Head,3);	 //链表的删除函数
      printf("删除后的链表:\n");
      display(Head);
      /*删除后链表的长度*/
      len = get_len(Head,len);   
      printf("此链表的长度为:\nlen = %d\n",len);     

      freelink(Head);    //链表的销毁函数

      free(Head);
      Head = NULL;

      return 0;
}

/*****************带节点的头插法创建链表*******************/
#if 0
struct Linked  *create()
{
      C   head,NODE;
      int n;        //链表的节点个数
      int i;        //链表节点的计数器

      printf("请输入需要创建的节点数目:");
      scanf("%d",&n);

      head = (C)malloc(sizeof(B));
      if(NULL == head)
      {
	    perror("head malloc failed!");
	    exit(1);
      }
      head->next = NULL;

      for(i = 0;i < n;i++)     //链表节点的创建条件
      {
	    NODE = (C)malloc(sizeof(B));   //节点申请空间
	    if(NULL == NODE)
	    {
		  perror("NODE申请空间失败!\n");
		  exit(1);
	    }

	    scanf("%d%s%f",&NODE->data.num,NODE->data.name,\
			&NODE->data.score);    //给节点数据域赋值

	    NODE->next = head->next;
	    head->next = NODE;
      }

      return head;    //返回创建链表的头指针
}

#else

/**************带节点的尾插法创建链表****************/
struct Linked *create()
{
      C head,tail,NODE;	//head头指针 tail尾指针
      int i;		//链表节点的计数器
      int n;		//链表的节点个数

      printf("请输入需要创建的节点数目：");
      scanf("%d",&n);

      head = (C)malloc(sizeof(B));
      if(NULL == head)
      {
	    perror("head malloc failed!");
	    exit(1);
      }

      tail = head;

      for(i = 0;i < n;i++)        //链表的创建条件
      {
	    NODE = (C)malloc(sizeof(B));
	    if(NULL == NODE)
	    {
		  perror("NODE malloc failed!");
		  exit(1);
	    }

	    scanf("%d%s%f",&NODE->data.num,NODE->data.name,\
			&NODE->data.score);    //给节点数据域赋值
	    
	    tail->next = NODE;
	    tail = NODE;
	    tail->next = NULL;
      }

      return head;         //返回创建链表的头指针
}
#endif

/*****************输出节点信息******************/
void  display(C Head)
{
      C  p;

      p = Head->next;
      while(p)
      {
	    printf("%d\t%s\t%f\n",p->data.num,p->data.name,\
			 p->data.score);     //输出节点的信息	    
	    p = p->next;
      }
}

/*****************插入链表节点******************/
     //将新节点插入节点i的后面
void insert(C Head, C newNODE ,int i)   
{
      C   p;     //当前指针
      int j;

      printf("请输入新插入节点的信息:\n");
      scanf("%d%s%f",&newNODE->data.num,newNODE->data.name,\
				      &newNODE->data.score);  
      p = Head;

      for(j=0;j<i && p!=NULL;j++)
	    p = p->next;      //p指向节点i
      if(NULL == p)
      {
	    printf("节点i不存在!\n");
	    return;
      }

      newNODE->next = p->next;   //将新节点的指针域指向节点i的后继节点
      p->next = newNODE;         //将节点i的指针域指向新的节点

}

/********************删除链表*******************/
void  delete(C Head,int i)
{

      C   p,q;
      int j;

      if(i == 0)    
	    return;
      p = Head;

      for(j=1;j<i && p->next!=NULL;j++)
	    p = p->next;      //p指向节点i的前驱节点
      if(NULL == p->next)
      {
	    printf("您需要删除的节点不存在!\n");
	    return;
      }

      q = p->next;     //q指向待删除的节点(节点i)
      p->next = q->next;     //删除节点i

      free(q);    //释放节点i的内存单元 
      q = NULL;    

}

/*******************销毁链表******************/
void  freelink(C Head)
{
      C   p,q;
      
      p = Head;
      
      while(p->next != NULL)  //依次释放头指针之后的节点内存
      {
	    q = p->next;      //q指向待释放的节点

	    p->next = q->next;   //指向待释放节点的后继节点

	    free(q);          //释放节点内存空间
	    q = NULL;
      }

}

/*****************链表的长度******************/
int  get_len(C Head,int len)
{
      C   p;

      len = 0;
      p = Head;

      while(NULL != p->next)
      {
	    len++;
	    p = p->next;
      }
      
    //  printf("此链表的长度为:\nlen = %d\n",len);

      return len;
}

/***************************************/


