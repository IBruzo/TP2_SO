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

void print_PCB(const PCB *pcb)
{
    print("PID: %d, RSP: %x\n", pcb->PID, pcb->RSP);
}
/* el kernel setea el primer valor de esta wea, luego en cada schedule se updatea, primer codigo del scheduler deberia verificar
que no es el pid 0, si lo es actualiza esta variable al siguiente y sigue su vida
hay que tener cuidado cuando se actualiza el pcb anterior */

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
