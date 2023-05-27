#ifndef _SCH_LIB_H_
#define _SCH_LIB_H_
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include "syscalls.h"
#include "list.h"
#include "dlc_list.h"

extern int dlcSize;
extern point *currentProcess;
extern point *previousProcess;

void buildPCB(PCB *block, int PID, int PPID, uint64_t RSB, char state, char priority, int FDArr[], int FDSize);
int cmpInt(PCB n1, PCB n2);
void list_print(list_t *list);

#endif
