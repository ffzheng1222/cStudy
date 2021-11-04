
#ifndef __DOUBLELIST_H
#define __DOUBLELIST_H	

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


typedef int datatype;
typedef struct doublelist{
      datatype data;
      struct doublelist *next;
      struct doublelist *prev;
}double_list,*double_plist;


extern void doublelist_init(double_plist *head);
extern void doublelist_create(double_plist head);
extern void doublelist_insert(double_plist head,double_plist new);
extern void doublelist_insert_tail(double_plist head,double_plist new);
extern void doublelist_del(double_plist head);
extern void doublelist_del_before(double_plist p);
extern void doublelist_del_after(double_plist p);
extern bool is_empty_doublelist(double_plist head);
extern void doublelist_sort(double_plist head);
extern void doublelist_show(double_plist head);

#endif
