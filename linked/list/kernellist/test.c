#include "kernellist.h"


int main(void)
{
      //定义头指针(一个指向头节点的指针)
      kernel_plist head;

      //初始化内核链表
      kernellist_init(&head);

      //创建内核链表
      kernellist_create(head);

      //对链表进行排序:奇数升序,偶数降序
      printf("对链表进行排序后(奇数升序,偶数降序)的输出:\n");
      kernellist_sort(head);

      return 0;
}
