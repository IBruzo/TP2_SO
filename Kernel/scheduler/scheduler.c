// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "scheduler.h"

/* Scheduler con Round Robin */

list_t *current;
Iterator *iterator;
int flag = 1;

void initScheduler()
{
    list_init(&route);                    // Creo la Ruta del Scheduler
    iterator = dlcCreateIterator(&route); // Iterador sobre los procesos
    current = dlcNext(iterator);
}

uint64_t schedule(uint64_t RSP)
{
    //  Se actualiza el PCB del Proceso Saliente
    PCB *aux = get(PCBTable, current->data);
    aux->RSP = RSP;
    // Si todavia tiene vidas le quito una y retorno su RSP
    if (aux->lives > 0 && aux->state == READY)
    {
        aux->lives--;
        return aux->RSP;
    }
    // Se quedo sin vida se las reinicio pero elijo a otro proceso
    aux->lives = aux->priority;
    // Si el proceso abandono por un bloqueo entonces no lo dejo en ready, y lo mismo un proceso recien terminado
    if (!flag && aux->state != EXITED)
    {
        aux->state = READY;
    }
    // Avanzamos el proceso entrante
    current = dlcNext(iterator);
    // Si es el nodo centinela lo ignoramos
    if (current->data == 0)
        current = dlcNext(iterator);
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

int block(int pid)
{
    PCB *blockedProcess = get(PCBTable, pid);
    if (blockedProcess->state == BLOCKED || blockedProcess->state == EXITED)
    {
        return -1;
    }

    int index = 0;
    Iterator *routeIt = dlcCreateIterator(&route);
    list_t *processIt;
    while (index < dlcSize + 1)
    {
        processIt = dlcNext(routeIt);
        if (processIt->data == blockedProcess->PID)
        {
            list_remove(processIt);
            list_t *toFree = processIt;
            memFree(toFree);
            dlcSize--;
            flag++;
            blockedProcess->state = BLOCKED;
            blockedProcess->lives = 0;
            return 1;
        }
        index++;
    }
    return -1;
}

int unblock(int pid)
{
    PCB *blockedProcess = get(PCBTable, pid);
    if (blockedProcess->state == EXITED)
    {
        return -1;
    }
    if (pid != -1 && flag)
    {
        list_t *newProcess = (list_t *)sys_mAlloc(sizeof(list_t));
        newProcess->data = pid;
        list_push(&route, newProcess);
        dlcSize++;
        flag--;
        blockedProcess->state = READY;
        blockedProcess->lives = blockedProcess->priority;
        return 1;
    }
    return -1;
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

// Printea la Ruta de procesos que sigue el scheduler
void printRoute()
{
    Iterator *routeIt = dlcCreateIterator(&route);
    for (int i = 0; i < dlcSize + 1; i++)
    {
        list_t *processIt = dlcNext(routeIt);
        print("||  PID  [%d]  ||  ", processIt->data);
    }
}
