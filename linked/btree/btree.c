#include "btree.h"
#include "linkqueue.h"
#include "seqstack.h"


/*******二叉树的链式存储*******/


//二叉树的两种创建方式

#if 0
btree_pnode btree_create(void)
{
      btree_pnode   T;
      datatype_btr ch;

      printf("请输入创建的树:");
      scanf("%c",&ch);

      if(ch == '#')
	    T = NULL;
      else
      { 
	    T = (btree_pnode)malloc(sizeof(btree_node));
	    if(NULL == T)
	    {
		  perror("T malloc failed!");
		  exit(1);
	    }

	    //递归创建二叉树的链式存储
	    T->data = ch;
	    T->lchild = btree_create();
	    T->rchild = btree_create();
      }

      return T;
}

#else

void btree_create(btree_pnode *T)
{
      datatype_btr ch;

      //      printf("请输入创建的树:");
      scanf("%c",&ch);

      if(ch == '#')
	    *T = NULL;
      else
      { 
	    *T = (btree_pnode)malloc(sizeof(btree_node));
	    if(NULL == *T)
	    {
		  perror("*T malloc failed!");
		  exit(1);
	    }

	    //递归创建二叉树的链式存储
	    (*T)->data = ch;
	    btree_create(&(*T)->lchild);
	    btree_create(&(*T)->rchild);
      }
}
#endif

/*二叉树的多种遍历方法*/

//(1):前序遍历
void pre_order(btree_pnode T)
{
      if(T != NULL)
      {
	    printf("%c",T->data);
	    pre_order(T->lchild);
	    pre_order(T->rchild);
      }
}
//(2):前序遍历----通过栈
void pre_stack_order(btree_pnode T)
{
      seq_pstack S;

      seqstack_init(&S);

      while(T != NULL || !is_empty_seqstack(S))      //1.判断树是否为空以及栈中否有元素
      {
	    while(T != NULL)
	    {
		  printf("%c",T->data);      //2.打印节点数据

		  if(T->rchild != NULL)      //3.若右子树不为空，则入栈
			seqstack_push(S,T->rchild);
		  T = T->lchild;          //4.指向根节点的指针移向左子树 
	    }
	    if(!is_empty_seqstack(S))     //5.若栈中有元素则出栈
		  seqstack_pop(S,&T);
      }
}

//(3):中序遍历
void mid_order(btree_pnode T)
{
      if(T != NULL)
      {
	    mid_order(T->lchild);
	    printf("%c",T->data);
	    mid_order(T->rchild);
      }
}

//(4):后序遍历
void post_order(btree_pnode T)
{
      if(T != NULL)
      {
	    post_order(T->lchild);
	    post_order(T->rchild);
	    printf("%c",T->data);
      }
}

//(5):按层遍历----通过队列
void level_order(btree_pnode T)
{
      link_pqueue Q;

      linkqueue_init(&Q);

      while(T != NULL)                         //1.判断根节点是否为空
      {
	    printf("%c",T->data); 
	    if(T->lchild != NULL)              //2.若左子树不为空，则入队      
		  linkqueue_in(Q,T->lchild);

	    if(T->rchild != NULL)              //3.若右子树不为空，则入队
		  linkqueue_in(Q,T->rchild);

	    if(!is_empty_linkqueue(Q))         //4.若队列不为空，则出队
		  linkqueue_out(Q,&T);
	    else
		  break;
      }
}

