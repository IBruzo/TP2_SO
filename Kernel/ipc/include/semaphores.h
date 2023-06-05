#include <stdint.h>
#include "lib.h"

#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#define MAX_NAME 15
#define MAX_SEM 20

typedef struct processNode
{
    struct processNode *next;
    int pid;
} process_t;

typedef struct
{
    char name[MAX_NAME];
    process_t *first;
    process_t *last;
    int lock;
    int value;
    int size;
    int listSize;
} sem_t;

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
