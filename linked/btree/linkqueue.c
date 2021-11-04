

#include "linkqueue.h"


/*的初始化循环队列*/
void linkqueue_init(link_pqueue *queue)
{
      //链队列头节点的空间申请
      *queue = (link_pqueue)malloc(sizeof(link_queue));  
      if(NULL == *queue)
      {
	    perror("*queue malloc failed!");
	    exit(1);
      }
      
      //给链队列的数据节点申请空间
      (*queue)->front = (link_pnode)malloc(sizeof(link_node));
      if(NULL == (*queue)->front)
      {
	    perror("(*queue)->front malloc failed!");
	    exit(1);
      }

      (*queue)->front->next = NULL;    //给链队列初始化
      (*queue)->rear = (*queue)->front ; 
}

/*入队*/
bool linkqueue_in(link_pqueue queue,datatype_qu data)
{
      link_pnode new;
      //给插入新结点申请空间
      new = (link_pnode)malloc(sizeof(link_node));
      if(NULL == new)
      {
	    perror("new  malloc failed!");
	    exit(1);
      }

      new->data = data;

      //入队的算法
      new->next = queue->rear->next;
      queue->rear->next = new;
      queue->rear = queue->rear->next;

      return true;
}

/*出队*/
bool linkqueue_out(link_pqueue queue,datatype_qu *D)
{
      link_pnode temp;

      if(is_empty_linkqueue(queue))
      {
	    printf("队为空,无法出队!\n");
	    return false;	    
      }

      //出队的算法
      temp = queue->front;
      queue->front = temp->next;
      *D = queue->front->data;
      free(temp);

      return true;
}

/*判断队列是否为空*/
bool is_empty_linkqueue(link_pqueue queue)
{
      if(queue->front == queue->rear)
	    return true;
      else
	    return false;

}

/*队列的遍历*/
#if 0
void linkqueue_show(link_pqueue queue)
{
      link_pnode p;

      for(p = queue->front->next; p != NULL; p = p->next)
	    printf("%d\t",p->data);

      printf("\n");
}
#endif

