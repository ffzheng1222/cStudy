#include "seqlist.h"


bool delete(seq_plist p,datatype data);
bool insert(seq_plist p,datatype data);


int main(void)
{
      datatype data;
      seq_plist p;     
      int ret;
  
      seqlist_init(&p);    //初始化顺序表

      while(1)
      {
	    printf("请输入一个整数:");
	    ret = scanf("%d",&data);

	    if(ret == 0)
	    {//输入的为字符
		  printf("good bey!\n");
		  break;
	    }
	    else if(data > 0)
	    {
		  insert(p,data);    //插入数据
		  seqlist_show(p);
	    }
	    else if(data < 0)
	    {
		  delete(p,-data);   //删除数据
		  seqlist_show(p);
	    }  
      }

      return 0;
}

bool insert(seq_plist p,datatype data)
{
      int i;
      //1.寻找要插入的位置
      for(i = 0;i <= p->last;i++)
	    if(data < p->data[i])
		  break;

      return seqlist_insert(p,i,data);  //2.调用插入函数
}

bool delete(seq_plist p,datatype data)
{
      int i;
      //1.寻找要删除的位置
      for(i = 0;i <= p->last;i++)
	    if(data == p->data[i])
		  break;
      if(i > p->last)
      {
	    printf("要删除的数据不存在!\n");
	    return false;
      }

      return seqlist_del(p,i);     //2.调用删除函数
}
