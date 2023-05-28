#include "scheduler.h"
#include "scheduler_lib.h"

list_t *current;
Iterator *iterator;
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
    aux->state = READY;

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
