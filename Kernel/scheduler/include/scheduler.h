#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
#include "lib.h"
#include <schedulerLib.h>
#include "stack.h"

uint64_t schedule(uint64_t RSP);
void initScheduler();
int block(int pid);
int unblock(int pid);
int getCurrentPPid();
int getCurrentPid();
struct list_t *getCurrentProcess();
int countCurrentProcessAppearances();
void printRoute();
extern int getTime(int op);
extern void forceTick();
void ps(char *);

#endif
