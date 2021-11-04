#include "seqstack.h"


/*顺序栈的初始化*/
void seqstack_init(seq_pstack *stack)
{
      *stack = (seq_pstack)malloc(sizeof(seq_stack));
      if(NULL == *stack)
      {
	    perror("*top malloc failed!");
	    exit(1);
      }

      (*stack)->top = -1; //初始化顺序栈
}

/*入栈*/
bool seqstack_push(seq_pstack stack,datatype_st data)
{
      if(is_full_seqstack(stack))
      {
	    printf("栈已满,不可入栈!\n");
	    return false;	    
      }

          //入栈的算法
      stack->top++;
      stack->data[stack->top] = data;

      return true;
}

/*出栈*/
bool seqstack_pop(seq_pstack stack,datatype_st *D)
{
      if(is_empty_seqstack(stack))
      {
	    printf("栈为空,无法入栈!\n");
	    return false;	    
      }

          //出栈的算法
      *D = stack->data[stack->top];
      stack->top--;

      return true;
}

/*判断栈是否已满*/
bool is_full_seqstack(seq_pstack stack)
{
      if(stack->top == SIZE-1)
	    return true;
      else
	    return false;
}

/*判断栈是否为空*/
bool is_empty_seqstack(seq_pstack stack)
{
      if(stack->top == -1)
	    return true;
      else
	    return false;

}

/*栈的遍历*/
#if 0
void seqstack_show(seq_pstack stack)
{
      int i;

      for(i = stack->top;i >= 0 ;i--)
	    printf("%d\t",stack->data[i]);

      printf("\n");
}
#endif

