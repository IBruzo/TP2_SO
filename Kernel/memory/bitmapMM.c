// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef BUDDY_MM

#include "memoryManager.h"

uint8_t bitMap[BIT_MAP_SIZE];
unsigned char bitMap[BIT_MAP_SIZE];

void *memStart;
uint32_t memSize;

uint32_t allocatedBytes; // en bytes

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
    memStart = hBase;
    memSize = hSize;

    memset(bitMap, 0, BIT_MAP_SIZE);
    memset(allocations, 0, BIT_MAP_SIZE * sizeof(Allocation));
    numAllocations = 0;
}

void switchBit(uint8_t *ch, int bitPos)
{
    *ch ^= (1 << bitPos);
}

// Switchea el valor del bit que se encuentra en position dentro del byte apuntado por byteDir
void switchBits(int posArr, int bitPos, int size)
{
    unsigned char mask = 128 >> bitPos;
    for (int i = bitPos; i < bitPos + size; i++)
    {
        int arrStep = i / 8;
        if (i % 8 == 0)
        {
            mask = 128;
        }
        switchBit(&bitMap[posArr + arrStep], 7 - i % 8);
        mask = mask >> 1;
    }
    return;
}

// Retorna la direccion donde comienza el bloque con el tamanio requerido
int findSpace(int cantPag, int *posArr, int *bitPos)
{

    int freeSpace = 0; // Variable que acumula la cantidad de memoria que se encontro hasta el momento
    int bitMapPosition;
    char bitPosition;
    int startArrPos = 0;
    char startBitPos = 0;
    for (bitMapPosition = 0; bitMapPosition < BIT_MAP_SIZE; bitMapPosition++)
    {
        unsigned char mask = 128;
        for (bitPosition = 0; bitPosition < 8; bitPosition++)
        {
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
            uint64_t dirMap = (((uint64_t)dir) - (uint64_t)(memStart)) / PAG_SIZE;
            int posArr = dirMap / 8;
            int bitPos = dirMap % 8;
            int cantPag = (allocations[i].size + PAG_SIZE - 1) / PAG_SIZE;
            switchBits(posArr, bitPos, cantPag);

            for (int j = i; j < numAllocations; j++)
            {
                allocations[j] = allocations[j + 1];
            }
            allocatedBytes -= cantPag * PAG_SIZE;
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
        void *address = (memStart + PAG_SIZE * (posArr * 8 + bitPos));
        allocations[numAllocations].address = address;
        allocations[numAllocations].size = sizeBytes;
        numAllocations++;
        allocatedBytes += cantPag * PAG_SIZE;
        return address;
    }
    return 0;
}

// Crea string de memoria total, ocupada y libre
void mem(char *buffer, int unit)
{
    int memSize = 8 * BIT_MAP_SIZE * PAG_SIZE;
    int total;
    int allocated;
    int free;
    char memStateString[150];
    if (unit == 0)
    { // mb
        int kibiConvert = 1024 * 1024;
        total = (int)memSize / kibiConvert;
        allocated = allocatedBytes / kibiConvert;
        free = (total - allocated);
        sprintf(memStateString, "Estado de la Memoria\n %d MB de memoria total\n %d MB en uso\n %d MB libres\n Para mayor precision usar el comando 'memb'\n", total, allocated, free);
    }
    else if (unit == 1)
    { // bytes
        total = memSize;
        allocated = allocatedBytes;
        free = (total - allocated);
        sprintf(memStateString, "Estado de la Memoria\n %d Bytes de memoria total\n %d Bytes en uso\n %d Bytes libres\n", total, (int)allocatedBytes, free);
    }
    else
    {
        sprintf(memStateString, "Unidad no reconocida\n");
    }
    strcpy(buffer, memStateString);
}

#endif
