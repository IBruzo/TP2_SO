#ifndef WAITSTACK_H
#define WAITSTACK_H

#define MAX_SIZE 100

#include "lib.h"

typedef struct
{
    int pid;
    int cpid;
} Process;

typedef struct
{
    Process data[MAX_SIZE];
    int top;
} WaitStack;

extern WaitStack waitQueue;

void initializeWaitStack(WaitStack *stack);
int isWaitStackEmpty(WaitStack *stack);
int isWaitStackFull(WaitStack *stack);
void pushWaitStack(WaitStack *stack, int pid, int cpid);
Process popWaitStack(WaitStack *stack);
Process peekWaitStack(WaitStack *stack);
void printWaitStack(WaitStack *stack);

#endif
