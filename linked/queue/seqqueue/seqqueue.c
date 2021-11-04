
#include "seqqueue.h"


/*的初始化循环队列*/
void seqqueue_init(seq_pqueue *queue)
{
      *queue = (seq_pqueue)malloc(sizeof(seq_queue));
      if(NULL == *queue)
      {
	    perror("*queue malloc failed!");
	    exit(1);
      }

      (*queue)->front = (*queue)->rear = SIZE-1; //初始化循环队列
}

/*入队*/
bool seqqueue_in(seq_pqueue queue,datatype data)
{
      if(is_full_seqqueue(queue))
      {
	    printf("队列已满,不可入队!\n");
	    return false;	    
      }

      //入队的算法
      queue->rear = (queue->rear+1) % SIZE ;
      queue->data[queue->rear] = data;

      return true;
}

/*出队*/
bool seqqueue_out(seq_pqueue queue,datatype *D)
{
      if(is_empty_seqqueue(queue))
      {
	    printf("队为空,无法出队!\n");
	    return false;	    
      }

      //出队的算法
      queue->front = (queue->front+1) % SIZE;
      *D = queue->data[queue->front];

      return true;
}

/*判断队列是否已满*/
bool is_full_seqqueue(seq_pqueue queue)
{
      if(queue->front == (queue->rear+1) % SIZE)
	    return true;
      else
	    return false;
}

/*判断队列是否为空*/
bool is_empty_seqqueue(seq_pqueue queue)
{
      if(queue->front == queue->rear)
	    return true;
      else
	    return false;

}

/*队列的遍历*/
void seqqueue_show(seq_pqueue queue)
{
      int i;

      /* 队列的越界处理 */
      for(i = (queue->front+1) % SIZE ; i != (queue->rear+1) % SIZE; i = (i+1) % SIZE)
	    printf("%d\t",queue->data[i]);

      printf("\n");
}


