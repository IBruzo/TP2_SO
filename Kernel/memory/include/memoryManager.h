#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <lib.h>
#include <stdint.h>
#include <stdarg.h>


/*********************************/

#include <lib.h>

#define PAG_SIZE 4096        // 4kb
#define BIT_MAP_SIZE (28672) // cantidad de memoria a mapear #pags

void initMemoryManager(void *hBase, uint32_t hSize);
void memFree(void *dir);
void *memAlloc(int size);
char * mem();

/*********************************/

#endif
