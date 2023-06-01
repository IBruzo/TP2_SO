#include <stdint.h>
#include "lib.h"

#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#define MAX_NAME 15
#define MAX_SEM 20

#define TRUE 1
#define FALSE 0

typedef struct processNode{
    struct processNode *next;
    uint64_t pid;
}  process_t;

typedef struct {
    char name[MAX_NAME];
    process_t *first;                   // 1er proceso en fila
    process_t *last;                    // Ultimo proceso en fila
    uint64_t lock;
    int value;
    uint64_t size;                      // cantidad de procesos que usan el semaforo
    uint64_t listSize;                  // cantidad de procesos bloqueados
} sem_t;

extern uint64_t _xchg(uint64_t *lock, int value);

void initSems();

uint64_t semCreate(char *name, uint64_t initValue);

uint64_t semOpen(char *name, uint64_t initValue);

uint64_t semClose(char *name);
// Locks a semaphore. If successful (the lock was acquired), sem_wait() and sem_trywait() will return 0.  Otherwise, -1 is returned and errno is set, and the state of the semaphore is unchanged.

uint64_t semWait(uint64_t semIndex);
// the value of the semaphore is incremented, and all threads which are waiting on the semaphore are awakened. If successful, sem_post() will return 0.  Otherwise, -1 is returned.

uint64_t semPost(uint64_t semIndex);

void sem();

char *getSemName(uint64_t semIndex);

uint64_t getSemIndex(char *name);

//void printProcessesSem(uint64_t semIndex);

#endif