#ifndef UNTITLED5_LISTADT_H
#define UNTITLED5_LISTADT_H

#include <stdint.h>

// hay que mover esto a otro.h
#define READY 0
#define RUNNING 1
#define BLOCKED 2
#define EXITED 3
// #define DUMMY 3

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
    uint64_t RSP;
    char state;
    char priority;
    int FD[256];
    int FDSize;
    memInfo memInfo;
} PCB;

typedef struct Node
{
    struct PCB *data;
    struct Node *next;
} Node;

typedef struct List
{
    Node *head;
    int size;
} List;

void newList(List *list);

void freeList(List *list);

int isEmpty(const List *list);

int size(const List *list);

void insert(List *list, PCB *data);

void delete(List *list, int targetPID);

PCB *get(List *list, int targetPID);

void iterate(List *list, void (*action)(const PCB *));

Node *next(Node *current);

Node *begin(List *list);

#endif // UNTITLED5_LISTADT_H
