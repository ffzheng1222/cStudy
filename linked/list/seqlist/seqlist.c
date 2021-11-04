
#include "seqlist.h"

//初始化数据
void seqlist_init(seq_plist *list)
{
      *list = (seq_plist)malloc(sizeof(seq_list));
      if(NULL == *list)
      {
	    perror("*list malloc failed!");
	    exit(1);
      }
      
      (*list)->last = -1;
}

//插入数据
bool seqlist_insert(seq_plist list,int i,datatype data)
{
      int j;
      if(is_full_seqlist(list))  //判断顺序表是否已满
      {
	    printf("插入失败,顺序表已满!\n");
	    return false;
      }

      //1.将i位置的后继元素依次后移动一位
      for(j = list->last;j >= i;j--) 
	    list->data[j+1] = list->data[j]; 

      //2.将待插入的数据data插入到i位置
      list->data[i] = data;

      //3.元素个数+1
      list->last++;

      return true;
      
}

//删除数据
bool seqlist_del(seq_plist list,int i)
{
      if(is_empty_seqlist(list))
      {
	    printf("顺序表为空,删除失败!\n");
	    return false;
      }
      if(i < 0 || i >= list->last)
      {
	    printf("i的位置不合法!\n");
	    return false;
      }

      //1.将i位置的后继元素依次向前覆盖一个位置
      int j;
      for(j = i;j < list->last;j++)
	    list->data[j] = list->data[j+1];

      //2.元素的个数-1
      list->last--;

      return true;
}

//判断顺序表是否为空
bool is_empty_seqlist(seq_plist list)
{
      if(list->last == -1)
	    return true;
      else
	    return false;
}

//判断顺序表是否已满
bool is_full_seqlist(seq_plist list)
{
      if(list->last == SIZE)
	    return true;
      else
	    return false;
}

//遍历顺序表
void seqlist_show(seq_plist list)
{
      int i;
      for(i = 0;i <= list->last;i++)
	    printf("%d\t",list->data[i]);
      printf("\n");
}
