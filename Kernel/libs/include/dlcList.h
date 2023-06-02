#ifndef _DLC_LIST_H_
#define _DLC_LIST_H_ // dlc = doubly linked circular
#include <memoryManager.h>
#define NULL ((void *)0)

typedef struct list_t
{
    struct list_t *prev;
    struct list_t *next;
    int data;
} list_t;

typedef struct Iterator
{
    list_t *current;
    list_t *end;
} Iterator;

extern list_t route;

void list_init(list_t *list);
void list_push(list_t *list, list_t *entry);
void list_remove(list_t *entry);
list_t *list_pop(list_t *list);

Iterator *dlcCreateIterator(list_t *list);
// int dlcHasNext(Iterator *iterator);
list_t *dlcNext(Iterator *iterator);
void dlcSkipNode(Iterator *iterator, list_t *node);
void dlcFreeIterator(Iterator *iterator);
void list_print(list_t *list, int numElements);

#endif
