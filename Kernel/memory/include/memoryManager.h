#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "memoryManager.h"
#include <dlc_list.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory);

void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate);

/* variable global de referencia la MemManager, para que la syscall pueda referenciarla */
extern MemoryManagerADT memManager;

/*********************************/

#include <lib.h>

#define PAG_SIZE 4096        // 4kb
#define BIT_MAP_SIZE (28672) // cantidad de memoria a mapear #pags

void initMemoryManager(void *hBase, uint32_t hSize);
void memFree(void *dir);
void *memAlloc(int size);

/*********************************/

#endif
