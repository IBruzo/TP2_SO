#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
#include "lib.h"
#include <scheduler_lib.h>

uint64_t schedule(uint64_t RSP);
void initScheduler();
void block();
void unblock();
int getCurrentPid();
struct list_t *getCurrentProcess();
int countCurrentProcessAppearances();
void printRoute();
extern int getTime(int op);
extern void forceTick();

#endif
