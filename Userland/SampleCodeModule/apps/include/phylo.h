#ifndef _PHYLO_H
#define _PHYLO_H

#include "library.h"
#include "console.h"
#define MAX_PHYL 10
#define MIN_PHYL 2
#define INIT_PHYL 5
#define SEM_PHYL "semPhylos"
#define LEN 10

enum state
{
    THINKING,
    EATING,
    WAITING
};

#define ROJO 0xFF0000
#define AMARILLO 0xFFFF00

int addPhylo();
int removePhylo();
void endtable();
void *phyloProcess(int argc, char *argv[]);
void printState();

void putForks(int i);
void takeForks(int i);
void resetState();
void test(int i);
#endif