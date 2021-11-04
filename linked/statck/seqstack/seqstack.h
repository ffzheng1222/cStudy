
#ifndef __SEQSTACK_H
#define __SEQSTACK_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 10

typedef int datatype;
typedef struct seqstack{
      datatype data[SIZE];
      int top;
}seq_stack,*seq_pstack;

extern void seqstack_init(seq_pstack *stack);
extern bool seqstack_push(seq_pstack stack,datatype data);
extern bool seqstack_pop(seq_pstack stack,datatype *D);
extern bool is_full_seqstack(seq_pstack stack);
extern bool is_empty_seqstack(seq_pstack stack);
extern void seqstack_show(seq_pstack stack);



#endif
