#include "scheduler.h"

list_t *current;
Iterator *iterator;
int flag = 1;

void initScheduler()
{
    list_init(&route);                    // Creo la Ruta del Scheduler
    iterator = dclCreateIterator(&route); // Iterador sobre los procesos
    current = dclNext(iterator);
}

uint64_t schedule(uint64_t RSP)
{
    // Se actualiza el PCB del Proceso Saliente
    PCB *aux = get(PCBTable, current->data);
    aux->RSP = RSP;
    if (flag)
    {
        aux->state = READY;
    }
    // Avanzamos el proceso entrante
    current = dclNext(iterator);
    // Si es el nodo centinela lo ignoramos
    if (current->data == 0)
        current = dclNext(iterator);
    // Se actualiza el PCB del Proceso Entrante y se retorna
    if (!dlcSize)
    {
        // Idle
        PCB *idle = get(PCBTable, 1);
        return idle->RSP;
    }
    else
    {
        // New Process
        PCB *aux2 = get(PCBTable, current->data);
        aux2->state = RUNNING;
        return aux2->RSP;
    }
}

void block()
{
    // print("Block\n");
    PCB *blockedProcess = get(PCBTable, current->data);
    blockedProcess->state = BLOCKED;

    flag = 0;

    int cantElim = 0; // found a killable process
    Iterator *routeIt = dclCreateIterator(&route);
    list_t *processIt;
    while (cantElim != blockedProcess->priority)
    {
        processIt = dclNext(routeIt);
        if (processIt->data == blockedProcess->PID)
        {
            list_remove(processIt);
            list_t *toFree = processIt;
            memFree(toFree);
            dlcSize--;
            cantElim++;
        }
    }
    return;
}

static int getBlockedPid()
{
    Node *curr = begin(PCBTable);
    while (curr != NULL)
    {
        if (curr->data->state == BLOCKED)
        {
            return curr->data->PID;
        }
        curr = next(curr);
    }
    return -1;
}

void unblock()
{
    // print("UNBlock\n");
    int pid = getBlockedPid();
    if (pid != -1 && !flag)
    {
        list_t *newProcess = (list_t *)sys_allocMem(sizeof(list_t));
        newProcess->data = pid;
        list_push(&route, newProcess);
        dlcSize++;
        flag = 1;
    }
}

int getCurrentPid()
{
    return current->data;
}

list_t *getCurrentProcess()
{
    return current;
}

int countCurrentProcessAppearances()
{
    int a = dlcSize;
    int count = 0;
    Iterator *routeIt = dclCreateIterator(&route);
    list_t *processIt;
    for (int i = 0; i < dlcSize + 1; i++)
    {
        processIt = dclNext(routeIt);
        if (processIt->data == getCurrentPid())
            count++;
    }
    return count;
}

void printRoute()
{
    Iterator *routeIt = dclCreateIterator(&route);
    list_t *processIt;
    for (int i = 0; i < 5; i++)
    {
        processIt = dclNext(routeIt);
        print("||  PID  [%d]  ||  ", processIt->data);
    }
}
