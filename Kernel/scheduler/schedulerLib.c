#include "schedulerLib.h"

int dlcSize = 0;

/* --------------------------------------- PCB FUNCTIONS -------------------------- */

void buildPCB(PCB *block, int PID, int PPID, uint64_t RSP, char state, char priority, int *FDArr)
{
    block->PID = PID;
    block->PPID = PPID;
    block->RSP = RSP;
    block->state = state;
    block->priority = priority;
    memcpy(block->FD, FDArr, sizeof(int) * 2);
    block->lives = 1;
    return;
}

void buildStartUpProcess(uint64_t *stackStart, void (*f)())
{
    // inicializo entradas de stack en 0
    for (int i = 0; i < 20; i++)
    {
        stackStart[i] = 0;
    }

    // valores especificos
    stackStart[15] = (uint64_t)f;                   // Entry Point de Idle
    stackStart[16] = 0x8;                           // Code Segment
    stackStart[17] = 0x202;                         // RFLAGS
    stackStart[18] = (uint64_t)(stackStart + 4096); // RBP previo a  la int
    stackStart[19] = 0;                             // Stack Segment
}

void idleProcess()
{
    while (1)
    {
        _hlt();
    }
}

void initializeStackFrame(int argc, char **argv, void *(*fn)(int, char **), uint64_t pid)
{
    PCB *currentProcess = get(PCBTable, pid);
    stackFrame *stack = (stackFrame *)(currentProcess->RSP);
    // Se incializan los registros con numeros seguidos, se hace mas facil para debuggear.
    stack->r15 = 0x001;
    stack->r14 = 0x002;
    stack->r13 = 0x003;
    stack->r12 = 0x004;
    stack->r11 = 0x005;
    stack->r10 = 0x006;
    stack->r9 = 0x007;
    stack->r8 = 0x008;
    // Se completa el stack con la informacion necesaria a la hora de ejecutar el proceso.
    stack->rsi = (uint64_t)argv;
    stack->rdi = argc;
    stack->rbp = 0;
    stack->rdx = (uint64_t)fn;
    stack->rcx = pid;
    stack->rip = (uint64_t)fn;
    stack->cs = 0x8;
    stack->rflags = 0x202;
    stack->rsp = (uint64_t)(currentProcess->RSP);
    stack->ss = 0x0;
}
