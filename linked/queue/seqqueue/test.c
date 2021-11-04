
#include "seqqueue.h"


int main(void)
{
      seq_pqueue queue;   //定义队列指针

      datatype data,temp;
      int ret;

      seqqueue_init(&queue);   //队列的初始化

      while(1)
      {
	    printf("请输入一个整数:");
	    ret = scanf("%d",&data);

	    if(ret == 1)
	    {
		  //入队
		  seqqueue_in(queue,data);
		  seqqueue_show(queue);
	    }		
	    else
	    {
		  //出队
		  if(seqqueue_out(queue,&temp))
			printf("%d\n",temp);
		  seqqueue_show(queue);
	    }

	    while(getchar() != '\n');
      }

      return 0;
}

