#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory);

void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate);

/* variable global de referencia la MemManager, para que la syscall pueda referenciarla */
extern MemoryManagerADT memManager;

/*********************************/

#include <lib.h>

#define BIT_MAP_SIZE 16384
#define PAG_SIZE 4096
#define MEM_START 0x10000000

void initMM();
void memFree(void *dir);
void * memAlloc(int size);

/*********************************/


#endif
