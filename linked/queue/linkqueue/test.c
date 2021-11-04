#include "linkqueue.h"


int main(void)
{
      link_pqueue queue;
      datatype data,temp;
      int ret;

      linkqueue_init(&queue);

      while(1)
      {
	    printf("请输入一个整数:");
	    ret = scanf("%d",&data);
	    if(ret == 1)
	    {     
		  //入队
		  if(linkqueue_in(queue,data))
			printf("入队的是:%d\n",data);
		  linkqueue_show(queue);
	    }
	    else
	    {
		  //出队
		  if(linkqueue_out(queue,&temp))
			printf("出队的是:%d\n",temp);
		  linkqueue_show(queue);

		  while(getchar() != '\n');
	    }
      }
      return 0;
}
