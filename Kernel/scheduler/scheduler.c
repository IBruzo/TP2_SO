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

/* el kernel setea el primer valor de esta wea, luego en cada schedule se updatea, primer codigo del scheduler deberia verificar
que no es el pid 0, si lo es actualiza esta variable al siguiente y sigue su vida
hay que tener cuidado cuando se actualiza el pcb anterior */

extern uint64_t schedule(uint64_t RSP)
{
    print("\n------------------------\n", RSP);
    print("parameter RSP  [%d]   --|--   ", RSP);
    print("list size  [%d]   --|--   ", dlcSize);

    /* --------------------- OUT-Update -------------------------- */

    // Se actualiza el PCB del Proceso Saliente
    toBegin(PCBTable);
    while (hasNext(PCBTable))
    {
        PCB elem = next(PCBTable);
        if (elem.PID == current->data)
        {
            elem.RSP = RSP;
            elem.state = READY;
        }
    }

    // Avanzamos el proceso entrante
    current = dclNext(iterator);
    // Si es el nodo centinela lo ignoramos
    if (current->data == 0)
        current = dclNext(iterator);

    if (!dlcSize)
    {
        /* --------------------- Idle -------------------------- */
        toBegin(PCBTable);
        while (hasNext(PCBTable))
        {
            PCB elem = next(PCBTable);
            if (elem.PID == 1 /* idle process */)
            {
                print("RETURN IDLE PROCESS RSP  [%d]", elem.RSP);
                return elem.RSP;
            }
        }
    }
    else
    {
        /* --------------------- Switch -------------------------- */
        toBegin(PCBTable);
        while (hasNext(PCBTable))
        {
            PCB elem = next(PCBTable);
            if (elem.PID == current->data /* idle process */)
            {
                elem.state = RUNNING;
                print("RETURN PID  [%d]", elem.PID);
                print("RETURN RSP  [%d]", elem.RSP);
                return elem.RSP;
            }
        }
    }
}
