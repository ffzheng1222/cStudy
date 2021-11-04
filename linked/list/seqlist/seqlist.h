#ifndef __SEQLIST_H
#define __SEQLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 10

typedef int datatype;
typedef struct seqlist{
      datatype data[SIZE];
      int last;
}seq_list,*seq_plist;

extern void seqlist_init(seq_plist *list);
extern bool seqlist_insert(seq_plist list,int i,datatype data);
extern bool seqlist_del(seq_plist list,int i);
extern bool is_empty_seqlist(seq_plist list);
extern bool is_full_seqlist(seq_plist list);
extern void seqlist_show(seq_plist list);


#endif
