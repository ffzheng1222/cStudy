#ifndef __LINKLIST_H
#define __LINKLIST_H	

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


typedef int datatype;
typedef struct linklist{
      datatype data;
      struct linklist *next;
}link_list,*link_plist;


extern void linklist_init(link_plist *head);
extern void linklist_create(link_plist head);
extern void linklist_insert(link_plist head,link_plist new);
//bool linklist_del(link_plist head,int i);
extern bool linklist_del(link_plist head);
extern bool is_empty_linklist(link_plist head);
extern void linklist_sort(link_plist head);
extern void linklist_show(link_plist head);

#endif
