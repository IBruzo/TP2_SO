#include "scheduler.h"

void initScheduler()
{
}

extern void schedule(uint64_t RSP)
{
    /*
    Antes de que el scheduler corra deberia crearse la shell o un proceso idle, yo creo que con la shell es suficiente,
    no se bien si haciendo que se cree en la primer instruccion del SampleCodeModule es suficiente, probablemente sea mas
    limpio hacer una funcion de asm que cree un idle sin llamar al scheduler ( o sea sin ponerla en marcha (int20h))  !!

    La lista circular va a contener los PIDs de los procesos readys, la consola/idle deberia estar siempre ready

    Entonces es seguro suponer que esta lista circular nunca va a estar vacia, a menos que se elimine el proceso de la
    consola/idle lo cual podemos evitar que se haga.

    Yo supongo que la funcion create/delete se ocupa de modificar la lista circular, mientras que el scheduler cicla
    sobre ella mindlessly, modificando RSBs de la tabla. Dentro de esta modificacion hay que meditar la prioridad, o sea
    repetir los procesos acorde a su prioridad para que reciban mas turnos del scheduler.

    El scheduler tambien se deberia encargar de switchear los RSB, guardando el viejo en su lugar correcto
    */

    /* Actualizo el Proceso Previo */
    list_t *previous = route.prev;
    point *previousPoint = container_of(previous, point, link);
    if (previousPoint->PID == 0) // por el nodo centinela?
    {
        previousPoint = container_of(previous->prev, point, link);
    }
    toBegin(PCBTable);
    while (hasNext(PCBTable))
    {
        PCB elem = next(PCBTable);
        if (elem.PID == previousPoint->PID)
        {
            elem.RSP = RSP;
            elem.state = READY;
        }
    }

    /* Actualizo el Proceso Entrante */
    list_t *current = route.next;
    point *currentPoint = container_of(current, point, link);
    if (currentPoint->PID == 0)
    {
        currentPoint = container_of(previous->next, point, link);
    }
    while (hasNext(PCBTable))
    {
        PCB elem = next(PCBTable);
        if (elem.PID == currentPoint->PID)
        {
            elem.state = RUNNING;
        }
    }
    return currentPoint->PID;

    // save old RSP, tengo que actualizar el RSP porque capaz el proceso movio su stack, como se que proceso corrio ultimo?
    // lo guardo en una variable del schedulesr? que tal el primer procesO? variabel global de currentProcess, previousProcess?
    // next en la lista
}

// container_of(current, point, link);
