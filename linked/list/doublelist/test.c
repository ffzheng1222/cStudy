#include "doublelist.h"


int main(void)
{
      //定义头指针(一个指向头节点的指针)
      double_plist head;

      //初始化双向循环链表
      doublelist_init(&head);

      //创建双向循环链表
      doublelist_create(head);

      //对链表进行排序:奇数升序,偶数降序
      printf("对链表进行排序后(奇数升序,偶数降序)的输出:\n");
      doublelist_sort(head);

      return 0;
}
