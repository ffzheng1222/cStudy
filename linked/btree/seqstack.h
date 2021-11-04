
#ifndef __SEQSTACK_H
#define __SEQSTACK_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "btree.h"

#define SIZE 50

typedef btree_pnode datatype_st;
typedef struct seqstack{
      datatype_st data[SIZE];
      int top;
}seq_stack,*seq_pstack;

extern void seqstack_init(seq_pstack *stack);
extern bool seqstack_push(seq_pstack stack,datatype_st data);
extern bool seqstack_pop(seq_pstack stack,datatype_st *D);
extern bool is_full_seqstack(seq_pstack stack);
extern bool is_empty_seqstack(seq_pstack stack);
//extern void seqstack_show(seq_pstack stack);



#endif
