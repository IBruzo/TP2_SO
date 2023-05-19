#ifndef _BITMAPADT_H_
#define _BITMAPADT_H_

#include <lib.h>

#define BIT_MAP_SIZE 16384
#define PAG_SIZE 4096
#define MEM_START 0x10000000

void initBitMap();
void switchBits(int posArr, int bitPos, int size);
int findSpace(int cantPag, int *posArr, int *bitPos);
void freeBits(void *dir, int size);
void * allocBits(int size);

#endif