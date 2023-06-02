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
    //  Se actualiza el PCB del Proceso Saliente
    PCB *aux = get(PCBTable, current->data);
    aux->RSP = RSP;
    // Si todavia tiene vidasm le quito una y lo mando
    if (aux->lives > 0 && aux->state == READY)
    {
        aux->lives--;
        return aux->RSP;
    }
    // Se quedo sin vida se las reinicio pero elijo a otro proceso
    aux->lives = aux->priority;
    // Si el proceso abandono por un bloqueo entonces no lo dejo en ready
    if (!flag)
    {
        aux->state = READY;
    }
    // Avanzamos el proceso entrante
    current = dclNext(iterator);
    // print("current PID : %d", current->data);
    // Si es el nodo centinela lo ignoramos
    if (current->data == 0)
        current = dclNext(iterator);
    // print("current PID2 : %d", current->data);

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
    // print("blocking %d -->", pid);
    PCB *blockedProcess = get(PCBTable, pid);
    blockedProcess->state = BLOCKED;

    int index = 0;
    Iterator *routeIt = dclCreateIterator(&route);
    list_t *processIt;
    while (index != dlcSize + 1)
    {
        processIt = dclNext(routeIt);
        if (processIt->data == blockedProcess->PID)
        {
            list_remove(processIt);
            list_t *toFree = processIt;
            memFree(toFree);
            dlcSize--;
            flag++;
            return;
        }
    }
    return;
}

void unblock(int pid)
{
    // printRoute();
    // print("unblocking %d  |-|", pid);
    if (pid != -1 && flag)
    {
        PCB *blockedProcess = get(PCBTable, pid);
        blockedProcess->state = READY;

        list_t *newProcess = (list_t *)sys_allocMem(sizeof(list_t));
        newProcess->data = pid;
        list_push(&route, newProcess);
        dlcSize++;
        flag--;
    }
    // printRoute();
    // printList(PCBTable);
    //  forceTick();
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
    for (int i = 0; i < dlcSize + 1; i++)
    {
        processIt = dclNext(routeIt);
        print("||  PID  [%d]  ||  ", processIt->data);
    }
}
