#include "linkstack.h"


int main(void)
{
      link_pstack top;   //定义栈指针
      datatype data,temp;

      linkstack_init(&top);   //栈的初始化

      printf("请输入一个十进制数:");
      scanf("%d",&data);
      temp = data;

      if(data < 0)
	    data = -data;
      while(data)
      {   
	    linkstack_push(&top,data % 8);
	    data = data / 8;
      }

#if 1
      if(temp > 0)
	    printf("%d--->0",temp);
      if(temp < 0)
	    printf("%d--->-0",-temp);
#endif

      while(!is_empty_linkstack(top))
      {
	    linkstack_pop(&top,&temp);
	    printf("%d",temp);
      }

      printf("\n");

      return 0;
}


