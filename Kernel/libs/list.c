// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
    Node *newNode = (Node *)sys_mAlloc(sizeof(Node));
    newNode->data = data;

    if (list->head == NULL || compare_PCB(data, list->head->data) < 0)
    {
        newNode->next = list->head;
        list->head = newNode;
    }
    else
    {
        Node *current = list->head;

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
                list->head = current->next;
            }
            else
            {
                prev->next = current->next;
            }

            sys_mFree(current);
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
        sys_mFree(temp);
    }
    list->head = NULL;
    list->size = 0;
}

Node *next(Node *current)
{
    if (current != NULL)
    {
        return current->next;
    }
    return NULL;
}

Node *begin(List *list)
{
    return list->head;
}

void printList(List *list)
{
    Node *current = list->head;
    print("\nPCB Table\n");
    while (current != NULL)
    {
        print("PID [%d]  PRIO [%d] STATE[%d] |", current->data->PID, current->data->priority, current->data->state);
        current = current->next;
    }
}

static char *stateStr(int state)
{
    switch (state)
    {
    case BLOCKED:
        return "BLOCKED";
    case READY:
        return "READY  ";
    case RUNNING:
        return "RUNNING";
    default:
        return "EXITED ";
    }
}

// Imprime la lista de todos los procesos con sus propiedades
void ps(char *buffer)
{
    char header[100];
    sprintf(header, "Process    |  ID  | Prioridad |   RSP   |   RBP    |  Context  | State \n");
    strcpy(buffer, header);
    char line[100];
    sprintf(line, "-------------------------------------------------------------------------\n");
    strcat(buffer, line);

    Node *pcb = begin(PCBTable);
    while (pcb != NULL)
    {
        if (pcb->data->state != EXITED)
        {
            char *process = (char *)sys_mAlloc(sizeof(char) * 200);
            if ((uint32_t)pcb->data->RBP == 0)
            {
                sprintf(process, "%s    |  %d   |     %d     | %x  | 0x%s |  %s       | %s \n", pcb->data->name, pcb->data->PID, pcb->data->priority, (uint32_t)pcb->data->RSP, "000000", (pcb->data->FD[0] == 0 && pcb->data->FD[1] == 1) ? "FG" : "BG", stateStr(pcb->data->state));
            }
            else
            {
                sprintf(process, "%s    |  %d   |     %d     | %x  | 0x%x |  %s       | %s \n", pcb->data->name, pcb->data->PID, pcb->data->priority, (uint32_t)pcb->data->RSP, (uint32_t)pcb->data->RBP, (pcb->data->FD[0] == 0 && pcb->data->FD[1] == 1) ? "FG" : "BG", stateStr(pcb->data->state));
            }
            strcat(buffer, process);
            sys_mFree(process);
        }
        pcb = next(pcb);
    }
    return;
}
