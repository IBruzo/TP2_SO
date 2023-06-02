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
    if (!flag)
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

void block(int pid)
{
  //  print("bloqued %d -->",pid);
    PCB *blockedProcess = get(PCBTable, pid);
    blockedProcess->state = BLOCKED;

    flag++;

    int cantElim = 0; 
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


void unblock(int pid)
{
 //  print("unbloqued %d     ",pid);
    if (pid != -1 && flag)
    {
        PCB *blockedProcess = get(PCBTable, pid);
        blockedProcess->state = READY;
        for (size_t i = 0; i < blockedProcess->priority; i++)
        {
            list_t *newProcess = (list_t *)sys_allocMem(sizeof(list_t));
            newProcess->data = pid;
            list_push(&route, newProcess);
            dlcSize++;
            flag--;
        }
    }
}

int getCurrentPid()
{
    return current->data;
}

int getCurrentPPid()
{
    PCB *aux = get(PCBTable, current->data);
    return aux->PPID;
}

list_t *getCurrentProcess()
{
    return current;
}

int countCurrentProcessAppearances()
{
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
