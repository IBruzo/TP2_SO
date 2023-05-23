#ifndef BUDDY_MM

#include "memoryManager.h"
#include <stdint.h>
#include <stdarg.h>

uint8_t bitMap[BIT_MAP_SIZE ];
uint32_t memStart;
uint32_t memSize;


typedef struct {
    void* address;
    size_t size;
} Allocation;
Allocation allocations[BIT_MAP_SIZE]; // Array to store allocated memory information
int numAllocations = 0; // Number of allocations made


void initMemoryManager(void * hBase, uint32_t hSize)
{
    if( hBase == NULL || hSize == 0 ){
        return;
    }
    memStart = (uint32_t)hBase;
    memSize = hSize;

    memset(bitMap, 0, BIT_MAP_SIZE);
    memset(allocations, 0,  BIT_MAP_SIZE);
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
    char mask = 128; // 1000 0000
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

            int posArr = (int)(allocations[i].address - memStart) / PAG_SIZE;
            int bitPos = 0;
            int cantPag = (allocations[i].size + PAG_SIZE - 1) / PAG_SIZE;
            switchBits(posArr, bitPos, cantPag);

            // Remove deallocated memory from allocations array
            for (int j = i; j < numAllocations ; j++)
            {
                //printf("allocations[%p:%ld] = allocations[%p:%ld]\n",allocations[j].address, allocations[j].size, allocations[j + 1].address, allocations[j+1].size);
                allocations[j] = allocations[j + 1];
            }
            
            numAllocations--;

            break;
        }
    }
}

void * memAlloc(int sizeBytes)
{
    int posArr = 0;
    int bitPos = 0;
    int cantPag = (sizeBytes + PAG_SIZE - 1) / PAG_SIZE;
    if (findSpace(cantPag, &posArr, &bitPos))
    {
        switchBits(posArr, bitPos, cantPag);
        void *address =(void *)(memStart + PAG_SIZE * posArr );
        allocations[numAllocations].address = address;
        allocations[numAllocations].size = sizeBytes;
        numAllocations++;

        return address;
    }
    return 0;
}




void printAllocs(int n){
    print("%d\t%d\n", n ,numAllocations);
    for(int i=0; i<BIT_MAP_SIZE  && allocations[i].address != NULL; i++)
    {
        print("allocations[%d]:\t [%x:%x]\n", i, allocations[i].address, allocations[i].size);
    }
}
void test()
{
	void *mem1=memAlloc(16);
	print("mem1: %x\n", mem1);

    void *mem2 = memAlloc(32);
    print("mem2: %x\n", mem2);
    
    void *mem3 = memAlloc(64);
    print("mem3: %x\n", mem3);


    // Allocate memory after freeing
    void *mem4 = memAlloc(16);
    print("mem4: %p\n", mem4);

    printAllocs(1);

    memFree(mem1);
    

    memFree(mem2);
    memFree(mem3);
    memFree(mem4);

    printAllocs(2);

    void *mem5=memAlloc(25651757);
    void *mem6=memAlloc(58926595);
    void *mem7=memAlloc(46616756);
    void *mem8=memAlloc(2230891);
    void *mem9=memAlloc(474271);
    void *mem10=memAlloc(7453);
    void *mem11=memAlloc(122505);
    void *mem12=memAlloc(177505);

    printAllocs(3);

    memFree(mem5);
    memFree(mem6);
    memFree(mem7);
    memFree(mem8);
    memFree(mem9);
    memFree(mem10);
    memFree(mem11);
    memFree(mem12);

    printAllocs(4);
    
    mem10 = memAlloc(14326);
    mem11 = memAlloc(14326);

    printAllocs(5);

    memFree(mem10);
    memFree(mem11);
    printAllocs(6);
	print("test");
}

#endif