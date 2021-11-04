#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct student{
      int num;
      char name[20];
      float score;
      struct student *next;
}st;

void menu(st *p);
int check(st* p);
st *input(void);

int main(void)
{
      st *head,*p;
      head = (st *)malloc(sizeof(st));
    //  head->next = (st *)malloc(sizeof(st));
      if(head == NULL)
      {
	    return -1;
      }
      p = head;
      menu(p);
      p = head;
      free(head); 
      head = NULL;
      return 0;
}

void menu(st *p)
{
      int n;
      p = NULL;
      do 
      {
	    printf("1:输入\t2:查看\t3:退出\n");

	    printf("请选择\t");
	    scanf("%d",&n);
	    if(n!=1 && n!=2 && n!=3)
		  printf("您的选择有误，请重新选择！\n");
	    switch(n)
	    {
		  case 1: p = input();	      break;
		  case 2: check(p);	      break;
		  case 3: exit(1);	    
	    }

      }while(1);
}

int check(st* p)
{
      st *p1 = p->next;
      if(p1 == NULL)
      {
	    printf("暂无学生信息,请选择1输入学生信息\n");
      }
      else
      {
	    printf("学号\t姓名\t成绩\n");
	    while(p1 != NULL)
	    {
		  printf("%d\t%s\t%f\n",p1->num,p1->name,p1->score);
		  p1 = p1->next;
	    }
      }
      return 0;
}

st *input()
{
      typedef st* ST;
      ST p,*list;
      int n;
      list = (ST *)malloc(sizeof(st));
      if(NULL ==list)
      {
	    perror("malloc fail!\n");
	    exit(1);
      }
      *list = (st *)malloc(sizeof(st));
      if(NULL == (*list))
      {
	    perror("malloc fail!\n");
	    exit(1);
      }
      (*list)->next = malloc(sizeof(st));
      if(NULL == (*list)->next)
      {
	    perror("malloc fail!\n");
	    exit(1);
      }
      p = (st *)malloc(sizeof(st));
      if(NULL == p)
      {
	    perror("malloc fail!\n");
	    exit(1);
      }
      (*list)->next = p;

      printf("您需要输入几位学生的信息：");
      scanf("%d",&n);

      while(n)
      {
	    p->next = (st *)malloc(sizeof(st));
	    if(NULL == p->next)
	    {
		  perror("malloc fail!\n");
		  exit(1);

	    }

	    scanf("%d%s%f",&p->num,p->name,&p->score);

	    p = (st *)malloc(sizeof(st));
	    if(NULL == p)
	    {
		  perror("malloc fail!\n");
		  exit(1);
	    }
	    p = p->next;
	    p->next = NULL;
	    n--;
      }

      return (*list);
}



