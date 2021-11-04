
#ifndef __LINKSTACK_H
#define __LINKSTACK_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef int datatype;
typedef struct linkstack{
      datatype data;
      struct linkstack *next;
}link_stack,*link_pstack;

extern void linkstack_init(link_pstack *top);
extern bool linkstack_push(link_pstack *top,datatype data);
extern bool linkstack_pop(link_pstack *top,datatype *D);
extern bool is_empty_linkstack(link_pstack top);
extern void linkstack_show(link_pstack top);


#endif
