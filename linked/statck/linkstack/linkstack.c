#include "linkstack.h"


/*链栈的初始化*/
void linkstack_init(link_pstack *top)
{
      *top = NULL; //初始化链栈
}

/*入栈*/
bool linkstack_push(link_pstack *top,datatype data)
{
      link_pstack new;

      new  = (link_pstack)malloc(sizeof(link_stack));
      if(NULL == new)
      {
	    perror("*new malloc failed!");
	    return false;
      }

      new->data = data;

      //入栈的算法
      new->next = *top;
      *top = new;

      return true;
}

/*出栈*/
bool linkstack_pop(link_pstack *top,datatype *D)
{
      link_pstack t;

      if(is_empty_linkstack(*top))
      {
	    printf("栈为空,无法出栈!\n");
	    return false;	    
      }

      //出栈的算法
      t = *top;
      *D = t->data;
      *top = (*top)->next;
      free(t);

      return true;
}

/*判断栈是否为空*/
bool is_empty_linkstack(link_pstack top)
{
      if(NULL  == top)
	    return true;
      else
	    return false;

}

/*栈的遍历*/
void linkstack_show(link_pstack top)
{
      link_pstack p;

      for(p = top ;p != NULL ;p = p->next )
	    printf("%d\t",p->data);

      printf("\n");
}


