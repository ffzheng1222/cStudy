
#ifndef __LINKQUEUE_H
#define __LINKQUEUE_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef int datatype;
typedef struct queuenode{
      datatype data;
      struct queuenode *next;
}link_node,*link_pnode;

typedef struct linkqueue{
      link_pnode front,rear;
}link_queue,*link_pqueue;


extern void linkqueue_init(link_pqueue *queue);
extern bool linkqueue_in(link_pqueue queue,datatype data);
extern bool linkqueue_out(link_pqueue queue,datatype *D);
//extern bool is_full_linkqueue(link_pqueue queue);
extern bool is_empty_linkqueue(link_pqueue queue);
extern void linkqueue_show(link_pqueue queue);



#endif
