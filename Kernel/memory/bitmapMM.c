#ifndef BUDDY_MM
#include "memoryManager.h"
#include <stdint.h>
#include <stdarg.h>

uint8_t bitMap[BIT_MAP_SIZE];
uint32_t memStart;
uint32_t memSize;

uint32_t allocated; // en bytes

typedef struct
{
    void *address;
    size_t size;
} Allocation;
Allocation allocations[BIT_MAP_SIZE];
int numAllocations = 0;

void initMemoryManager(void *hBase, uint32_t hSize)
{
    if (hBase == NULL || hSize == 0)
    {
        return;
    }
    memStart = (uint32_t)hBase;
    memSize = hSize;

    memset(bitMap, 0, BIT_MAP_SIZE);
    memset(allocations, 0, BIT_MAP_SIZE);
    numAllocations = 0;
}

void switchBit(char *ch, int bitPos)
{
    *ch ^= (1 << bitPos);
}

// funcion que switchea el valor del bit que se encuentra en position dentro del byte apuntado por byteDir
void switchBits(int posArr, int bitPos, int size)
{
    // ejemplo switchBits(8, 5, 14); se quiere switchear los 15 bits comenzando del 8-5 al 10-4
    unsigned char mask = 128 >> bitPos;          // la mascara se setea en 0000 0100
    for (int i = bitPos; i < bitPos + size; i++) // i = 5 -> 19
    {
        int arrStep = i / 8; //  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2
        if (i % 8 == 0)
        {
            mask = 128;
        }
        switchBit(&bitMap[posArr + arrStep], 7 - i % 8);
        mask = mask >> 1;
    }
    return;
}

// funcion que retorna la direccion donde comienza el bloque con el tamanio requerido
int findSpace(int cantPag, int *posArr, int *bitPos)
{
    int freeSpace = 0; // variable que acumula la cantidad de memoria que se encontro hasta el momento
    int bitMapPosition;
    char bitPosition;
    int startArrPos = 0;
    char startBitPos = 0;
    unsigned char mask = 128; // 1000 0000
    for (bitMapPosition = 0; bitMapPosition < BIT_MAP_SIZE; bitMapPosition++)
    {
        for (bitPosition = 0; bitPosition < 8; bitPosition++)
        {
            // printf("%d--\n",!(bitMap[bitMapPosition] & mask));
            // bitMap[bitMapPosition] = 1100 0001
            // bitPosition 1000 0000 -> 0100 0000 -> 0010 0000 -> 0001 0000 -> 0000 1000 -> 0000 0100 -> 0000 0010 -> 0000 0001
            if (freeSpace == 0)
            {
                startArrPos = bitMapPosition;
                startBitPos = bitPosition;
            }
            if ((bitMap[bitMapPosition] & mask))
            {
                freeSpace = 0;
            }
            else
            {
                freeSpace++;
            }
            if (freeSpace == cantPag)
            {
                *posArr = startArrPos;
                *bitPos = startBitPos;
                return 1;
            }
            mask = mask >> 1;
        }
    }
    return 0;
}

void memFree(void *dir)
{
    for (int i = 0; i < numAllocations; i++)
    {
        if (allocations[i].address == dir)
        {
            int dirMap = (((int)dir) - memStart) / PAG_SIZE; // base + 4k*(8*posArr + bitPoss) bitPos[ 0-7 ]
            int posArr = dirMap / 8;                         // se trunca
            int bitPos = dirMap % 8;

            /* int posArr = (int)(allocations[i].address - memStart) / PAG_SIZE;
            int bitPos = 0; */
            int cantPag = (allocations[i].size + PAG_SIZE - 1) / PAG_SIZE;
            switchBits(posArr, bitPos, cantPag);

            // Remove deallocated memory from allocations array
            for (int j = i; j < numAllocations; j++)
            {
                // printf("allocations[%p:%ld] = allocations[%p:%ld]\n",allocations[j].address, allocations[j].size, allocations[j + 1].address, allocations[j+1].size);
                allocations[j] = allocations[j + 1];
            }
            allocated -= cantPag * PAG_SIZE;
            numAllocations--;

            return;
        }
    }
}

void *memAlloc(int sizeBytes)
{
    int posArr = 0;
    int bitPos = 0;
    int cantPag = (sizeBytes + PAG_SIZE - 1) / PAG_SIZE;
    if (findSpace(cantPag, &posArr, &bitPos))
    {
        switchBits(posArr, bitPos, cantPag);
        void *address = (void *)(memStart + PAG_SIZE * (posArr * 8 + bitPos));
        allocations[numAllocations].address = address;
        allocations[numAllocations].size = sizeBytes;
        numAllocations++;
        allocated += cantPag * PAG_SIZE;
        return address;
    }
    return 0;
}

void printMemState()
{
    // cant de memoria usada total libre total
    int total = 8 * BIT_MAP_SIZE * PAG_SIZE;

    print("total: %d-bytes  used: %d-bytes free: %d-bytes\n", total, allocated, total - allocated);
}

#endif
