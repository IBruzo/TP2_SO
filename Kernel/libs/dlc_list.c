
#include <dlc_list.h>
#include "lib.h"

list_t route;

void list_init(list_t *list)
{
  list->prev = list;
  list->next = list;
}

void list_push(list_t *list, list_t *entry)
{
  list_t *prev = list->prev;
  entry->prev = prev;
  entry->next = list;
  prev->next = entry;
  list->prev = entry;
}

void list_remove(list_t *entry)
{
  list_t *prev = entry->prev;
  list_t *next = entry->next;
  prev->next = next;
  next->prev = prev;
}

list_t *list_pop(list_t *list)
{
  list_t *back = list->prev;
  if (back == list)
    return NULL;
  list_remove(back);
  return back;
}

Iterator *dclCreateIterator(list_t *list)
{
  Iterator *iterator = (Iterator *)memAlloc(sizeof(Iterator));
  if (iterator == NULL)
  {
    print("Memory allocation failed.\n");
    return NULL;
  }
  iterator->current = list;
  iterator->end = list;
  return iterator;
}

list_t *dclNext(Iterator *iterator)
{
  list_t *current = iterator->current;
  iterator->current = current->next;
  return current;
}

void dclSkipNode(Iterator *iterator, list_t *node)
{
  if (iterator == NULL || node == NULL)
    return;

  if (iterator->current == node)
  {
    iterator->current = node->next;
  }
  list_remove(node);
}

void dclFreeIterator(Iterator *iterator)
{
  memFree(iterator);
}
