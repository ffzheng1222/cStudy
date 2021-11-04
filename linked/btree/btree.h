#ifndef __BTREE_H
#define __BTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef char datatype_btr;
typedef struct btreenode{
      datatype_btr data;
      struct btreenode *lchild,*rchild;
}btree_node,*btree_pnode;


extern void btree_create(btree_pnode *T);
extern void pre_order(btree_pnode T);
extern void mid_order(btree_pnode T);
extern void post_order(btree_pnode T);
extern void level_order(btree_pnode T);
extern void pre_stack_order(btree_pnode T);



#endif
