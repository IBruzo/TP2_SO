#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory);

void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate);

/* variable global de referencia la MemManager, para que la syscall pueda referenciarla */
extern MemoryManagerADT memManager;

#endif
