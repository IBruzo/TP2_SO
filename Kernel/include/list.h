#ifndef UNTITLED5_LISTADT_H
#define UNTITLED5_LISTADT_H

#include <stdint.h>

typedef struct memInfo
{
    int qPages;
    uintptr_t baseAddress;
    uintptr_t limit;
} memInfo;

typedef struct PCB
{
    int PID;
    int PPID;
    uintptr_t RSB;
    char state;
    char priority;
    int FD[256];
    int FDSize;
    memInfo memInfo;
} PCB;

typedef struct listCDT *listADT;

typedef PCB *elemType;

listADT newList(int (*compare)(elemType e1, elemType e2));

void freeList(listADT list);

int isEmpty(const listADT list);

int size(const listADT list);

// 1 si la inserta, 0 sinó (ya estaba)
int insert(listADT list, elemType elem);

// 1 si estaba, 0 sinó
int delete(listADT list, elemType elem);

int belongs(const listADT list, elemType elem);

elemType get(const listADT list, unsigned int idx);

void map(listADT list, elemType (*funcion)(elemType));

// Para comenzar a iterar
void toBegin(listADT list);
int hasNext(const listADT list);
elemType next(listADT list);

#endif // UNTITLED5_LISTADT_H
