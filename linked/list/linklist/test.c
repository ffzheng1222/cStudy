#include "linklist.h"


int main(void)
{
      link_plist head;

      linklist_init(&head);

      linklist_create(head);

      printf("链表的逆转输出:\n");
      linklist_sort(head);

      return 0;
}
