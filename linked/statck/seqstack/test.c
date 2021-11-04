#include "seqstack.h"


int main(void)
{
      seq_pstack stack;   //定义栈指针
      datatype data,temp;

      seqstack_init(&stack);   //栈的初始化

      printf("请输入一个十进制数:");
      scanf("%d",&data);
      temp = data;

      if(data < 0)
	    data = -data;
      while(data)
      {   
	    seqstack_push(stack,data % 8);
	    data = data / 8;
      }

#if 1
      if(temp > 0)
	    printf("%d--->0",temp);
      if(temp < 0)
	    printf("%d--->-0",-temp);
#endif

      while(!is_empty_seqstack(stack))
      {
	    seqstack_pop(stack,&temp);
	    printf("%d",temp);
      }

      printf("\n");

      return 0;
}


