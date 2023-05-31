#include "scheduler.h"
#include "scheduler_lib.h"

list_t *current;
Iterator *iterator;
int flag=1;

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
    if(flag){
         aux->state = READY;
    }

    // Avanzamos el proceso entrante
    current = dclNext(iterator);
    // Si es el nodo centinela lo ignoramos
    if (current->data == 0)
        current = dclNext(iterator);
    // Se actualiza el PCB del Proceso Entrante y se retorna
   // print("CURRENT PID %d", current->data);

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
      //  print(" PID RETURNING  [%d]", aux2->PID);
        return aux2->RSP;
    }
}


void block(){
    PCB * aux = get(PCBTable, current->data);
    aux->state = BLOCKED;
    list_remove(current);
    flag=0;
    dlcSize--;

}

static int getBlockedPid(){
    Node * curr=PCBTable->head;
    for(int i =0; i!=PCBTable->size;i++){
        PCB * a = curr->data;
        if(curr->data->state == BLOCKED){
            return curr->data->PID;
        }
        curr=curr->next;
    }
    return -1;
}

void unblock(){
    int pid= getBlockedPid();
    if(pid!=-1){
        list_t *newProcess = (list_t *)sys_allocMem(sizeof(list_t));
        newProcess->data = pid;
        list_push(&route, newProcess);
        dlcSize++;
        flag=1;
    }
    
}