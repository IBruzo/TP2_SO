#ifndef _BITMAPADT_H_
#define _BITMAPADT_H_

#include <lib.h>

#define BIT_MAP_SIZE 16384
#define PAG_SIZE 4096
#define MEM_START 0x50000

void initBitMap();
void switchBits(int posArr, int bitPos, int size);
int findSpace(int cantPag, int *posArr, int *bitPos);
void free(void *dir, int size);
void * alloc(int size);

#endif