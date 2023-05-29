#ifndef _SCH_LIB_H_
#define _SCH_LIB_H_
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include "syscalls.h"
#include "list.h"
#include "dlc_list.h"

extern List *PCBTable;
extern int dlcSize;
extern point *currentProcess;
extern point *previousProcess;

typedef struct stackFrame
{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rsi; // argv
    uint64_t rdi; // argc
    uint64_t rbp;
    uint64_t rdx; // function
    uint64_t rcx; // pid
    uint64_t rbx;
    uint64_t rax;
    uint64_t rip;    // loader
    uint64_t cs;     // 0x8
    uint64_t rflags; // 0x202
    uint64_t rsp;    // rbp
    uint64_t ss;     // 0
} stackFrame;

void buildPCB(PCB *block, int PID, int PPID, uint64_t RSB, char state, char priority, int FDArr[], int FDSize);
void buildStartUpProcess(uint64_t *stackStart, void (*f)());
void idleProcess();
void initializeStackFrame(int argc, char **argv, void * (*fn)(int, char **), uint64_t pid);

#endif
