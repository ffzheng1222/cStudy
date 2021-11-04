#include "btree.h"


int main(void)
{
      btree_pnode  T;
      
      btree_create(&T);

      printf("先序遍历:");
      pre_order(T);
      printf("\n");
      printf("通过栈----先序遍历:");
      pre_stack_order(T);
      printf("\n");
       
      printf("中序遍历:");
      mid_order(T);
      printf("\n");

      printf("后序遍历:");
      post_order(T);
      printf("\n");

      printf("通过队列---按层遍历:");
      level_order(T);
      printf("\n");
      return 0;
      
}
