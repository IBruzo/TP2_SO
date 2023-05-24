#include "scheduler_lib.h"
/* --------------------------------------- PCB FUNCTIONS -------------------------- */

void buildPCB(PCB *block, int PID, int PPID, uint64_t RSP, char state, char priority, int FDArr[], int FDSize)
{
    block->PID = PID;
    block->PPID = PPID;
    block->RSP = RSP;
    block->state = state;
    block->priority = priority;
    memcpy(block->FD, FDArr, sizeof(int) * FDSize);
    block->FDSize = FDSize;
    return;
}

void printPCBTable(listADT PCBTable)
{
    toBegin(PCBTable);
    while (hasNext(PCBTable))
    {
        PCB elem = next(PCBTable);
        print("PID    [%d]  --  PPID  [%d]  --  PRIORITY  [%d]  --  STATE  ", elem.PID, elem.PPID, elem.priority);
        switch (elem.state)
        {
        case 0:
            print("[READY]\n");
            break;
        case 1:
            print("[RUNNING]\n");
            break;
        case 2:
            print("[BLOCKED]\n");
            break;
        default:
            break;
        }
        print("RSP    [%d]\n", elem.RSP);
        print("FD");
        for (int i = 0; i < elem.FDSize; i++)
        {
            print("     [%d]", elem.FD[i]);
        }
        print("\n-------------------------------------------------------------------\n");
    }
}

int cmpInt(PCB n1, PCB n2)
{
    return n1.PID - n2.PID;
}

void list_print(list_t *list)
{
    list_t *current = list->next;
    for (int i = 0; i < 12; i++)
    {
        point *currentPoint = container_of(current, point, link);
        print("Checkpoint  [%d]\n", i);
        print("PID         [%d]\n", currentPoint->PID);
        print("--------------------\n", currentPoint->PID);

        current = current->next;
    }
}
