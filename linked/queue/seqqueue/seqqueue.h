

#ifndef __SEQQUEUE_H
#define __SEQQUEUE_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 20

typedef int datatype;
typedef struct seqqueue{
      datatype data[SIZE];
      int front,rear;
}seq_queue,*seq_pqueue;

extern void seqqueue_init(seq_pqueue *queue);
extern bool seqqueue_in(seq_pqueue queue,datatype data);
extern bool seqqueue_out(seq_pqueue queue,datatype *D);
extern bool is_full_seqqueue(seq_pqueue queue);
extern bool is_empty_seqqueue(seq_pqueue queue);
extern void seqqueue_show(seq_pqueue queue);



#endif
