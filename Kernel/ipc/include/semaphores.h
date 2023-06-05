#include <stdint.h>
#include "lib.h"

#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#define MAX_NAME 15
#define MAX_SEM 20

extern int _xchg(int *lock, int value);
void initSems();
int semCreate(char *name, int initValue);
int semOpen(char *name, int initValue);
int semClose(char *name);
int semWait(int semIndex);
int semPost(int semIndex);
char *getSemName(int semIndex);
int findSem(char *name);

#endif
