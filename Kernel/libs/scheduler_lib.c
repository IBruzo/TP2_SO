#include "scheduler_lib.h"

int dlcSize = 0;

point *currentProcess;
point *previousProcess;

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

void buildStartUpProcess(uint64_t *stackStart, void (*f)())
{
    /* construyendole de abajo para arriba el stack, contrario al push */
    stackStart[0] = 0;                              // R15
    stackStart[1] = 0;                              // R14
    stackStart[2] = 0;                              // R13
    stackStart[3] = 0;                              // R12
    stackStart[4] = 0;                              // R11
    stackStart[5] = 0;                              // R10
    stackStart[6] = 0;                              // R9
    stackStart[7] = 0;                              // R8
    stackStart[8] = 0;                              // RSI, no necesita parametros
    stackStart[9] = 0;                              // RDI, no necesita parametros
    stackStart[10] = 0;                             // RBP
    stackStart[11] = 0;                             // RDX
    stackStart[12] = 0;                             // RCX
    stackStart[13] = 0;                             // RBX
    stackStart[14] = 0;                             // RAX
    stackStart[15] = (uint64_t)f;                   // Entry Point de Idle
    stackStart[16] = 0x8;                           // Code Segment
    stackStart[17] = 0x202;                         // RFLAGS
    stackStart[18] = (uint64_t)(stackStart + 4096); // RBP previo a la int
    stackStart[19] = 0;                             // Stack Segment
    return;
}

void idleProcess()
{
    print("Chilling...\n");
    while (1)
    {
        _hlt();
    }
}
