#ifndef _PHYLO_H
#define _PHYLO_H

#include "library.h"

#define MAX_PHYL 15
#define MIN_PHYL 2
#define INIT_PHYL 5
#define SEM_PHYL "semPhylos"
#define LEN 10

typedef struct
{
    uint64_t pid;
    int state;
    uint64_t semIndex;
    char semName[LEN];
} phylo_t;

enum state
{
    THINKING,
    EATING,
    WAITING
};



int addPhylo(int pIndex);
int removePhylo(int pIndex);
void endTable();
void * phyloProcess(int argc, char *argv[]);
void printState();
void takeChopstick(int pIndex);
void update(int pIndex);
int left(int pIndex);
int right(int pIndex);


#endif