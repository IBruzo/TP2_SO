#ifndef _DLC_LIST_H_
#define _DLC_LIST_H_

#define NULL ((void *)0)

typedef struct list_t {
  struct list_t *prev, *next;
} list_t;

void list_init(list_t *list) ;
void list_push(list_t *list, list_t *entry) ;
void list_remove(list_t *entry) ;
list_t *list_pop(list_t *list) ;

#endif
