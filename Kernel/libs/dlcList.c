// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <dlcList.h>
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

Iterator *dlcCreateIterator(list_t *list)
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

list_t *dlcNext(Iterator *iterator)
{
  list_t *current = iterator->current;
  iterator->current = current->next;
  return current;
}

void dlcSkipNode(Iterator *iterator, list_t *node)
{
  if (iterator == NULL || node == NULL)
    return;

  if (iterator->current == node)
  {
    iterator->current = node->next;
  }
  list_remove(node);
}

void dlcFreeIterator(Iterator *iterator)
{
  memFree(iterator);
}

void list_print(list_t *list, int numElements)
{
  print("Scheduler List\n");
  if (list == NULL)
  {
    print("Invalid list.\n");
    return;
  }

  Iterator *printIterator = dlcCreateIterator(list);
  if (printIterator == NULL)
  {
    print("Failed to create iterator.\n");
    return;
  }

  int count = 0;
  list_t *node;
  while ((node = dlcNext(printIterator)) != NULL && count < numElements)
  {
    print("Element: %d  |", node->data);
    count++;
  }

  dlcFreeIterator(printIterator);
  print("\n");
}
