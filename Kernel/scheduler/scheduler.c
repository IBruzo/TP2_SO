// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "scheduler.h"

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
    // Si todavia tiene vidasm le quito una y lo mando
    if (aux->lives > 0 && aux->state == READY)
    {
        aux->lives--;
        return aux->RSP;
    }
    // Se quedo sin vida se las reinicio pero elijo a otro proceso
    aux->lives = aux->priority;
    // Si el proceso abandono por un bloqueo entonces no lo dejo en ready
    if (!flag && aux->state != EXITED)
    {
        aux->state = READY;
    }
    // Avanzamos el proceso entrante
    current = dlcNext(iterator);
    // print("current PID : %d", current->data);
    // Si es el nodo centinela lo ignoramos
    if (current->data == 0)
        current = dlcNext(iterator);
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

int block(int pid)
{
    // print("blocking %d -->", pid);
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
    // printRoute();
    // print("unblocking %d  |-|", pid);
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
    // printRoute();
    // printList(PCBTable);
    //  forceTick();
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

int countCurrentProcessAppearances()
{
    int count = 0;
    Iterator *routeIt = dlcCreateIterator(&route);
    list_t *processIt;
    for (int i = 0; i < dlcSize + 1; i++)
    {
        processIt = dlcNext(routeIt);
        if (processIt->data == getCurrentPid())
            count++;
    }
    return count;
}

void printRoute()
{
    Iterator *routeIt = dlcCreateIterator(&route);
    list_t *processIt;
    for (int i = 0; i < dlcSize + 1; i++)
    {
        processIt = dlcNext(routeIt);
        print("||  PID  [%d]  ||  ", processIt->data);
    }
}

/*
    Imprime la lista de todos los procesos con sus propiedades:
    nombre,
    ID,
    prioridad,
    stack y base pointer,
    foreground
    y cualquier otra variable que consideren necesaria.
*/

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

void ps(char *buffer)
{
    /* print header */
    char header[100];
    sprintf(header, "Process | ID  | Prioridad |   Stack   |   Base   |  Context  | State \n");
    strcpy(buffer, header);
    char line[100];
    sprintf(line, "--------------------------------------------------------\n");
    strcat(buffer, line);

    Node *pcb = begin(PCBTable);
    while (pcb != NULL)
    {
        if (pcb->data->state != EXITED)
        {
            char *process = (char *)sys_mAlloc(sizeof(char) * 200);
            sprintf(process, "%s    |  %d  |     %d    |%x     |%x    |  %s  |%s \n", pcb->data->name, pcb->data->PID, pcb->data->priority, (uint32_t)pcb->data->RSP, (uint32_t)pcb->data->RBP, (pcb->data->FD[0] == 0 && pcb->data->FD[1] == 1) ? "FG" : "BG", stateStr(pcb->data->state));
            strcat(buffer, process);
            sys_mFree(process);
        }
        pcb = next(pcb);
    }
    return;
}
