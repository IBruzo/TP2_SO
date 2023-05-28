#include "list.h"
#include "lib.h"
#include <assert.h>
#include "memoryManager.h"

void newList(List *list)
{
    list->head = NULL;
    list->size = 0;
}

static int compare_PCB(const PCB *a, const PCB *b)
{
    return a->PID - b->PID;
}

void insert(List *list, PCB *data)
{
    Node *newNode = (Node *)sys_allocMem(sizeof(Node));
    newNode->data = data;

    // If the list is empty or the new element should be inserted at the beginning
    if (list->head == NULL || compare_PCB(data, list->head->data) < 0)
    {
        newNode->next = list->head;
        list->head = newNode;
    }
    else
    {
        Node *current = list->head;

        // Traverse the list to find the appropriate position for insertion
        while (current->next != NULL && compare_PCB(data, current->next->data) >= 0)
        {
            current = current->next;
        }

        newNode->next = current->next;
        current->next = newNode;
    }

    list->size++;
}

void delete(List *list, int targetPID)
{
    Node *current = list->head;
    Node *prev = NULL;

    while (current != NULL)
    {
        if (current->data->PID == targetPID)
        {
            if (prev == NULL)
            {
                // Deleting the head node
                list->head = current->next;
            }
            else
            {
                // Deleting a node in the middle or at the end
                prev->next = current->next;
            }

            sys_free(current);
            list->size--;
            return;
        }

        prev = current;
        current = current->next;
    }
}

PCB *get(List *list, int targetPID)
{
    Node *current = list->head;
    while (current != NULL)
    {
        if (current->data->PID == targetPID)
        {
            return current->data;
        }
        current = current->next;
    }
    return NULL;
}

void iterate(List *list, void (*action)(const PCB *))
{
    Node *current = list->head;
    while (current != NULL)
    {
        action(current->data);
        current = current->next;
    }
}

int size(const List *list)
{
    return list->size;
}

int isEmpty(const List *list)
{
    return ((list->head) == NULL);
}

void freeList(List *list)
{
    Node *current = list->head;
    while (current != NULL)
    {
        Node *temp = current;
        current = current->next;
        sys_free(temp);
    }
    list->head = NULL;
    list->size = 0;
}
