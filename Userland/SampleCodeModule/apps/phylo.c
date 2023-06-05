#include "phylo.h"

static phylo_t phylos[MAX_PHYL];
static int seated;
uint64_t semphy;

#define ROJO 0xFF0000
void * phyloProcess(int argc, char *argv[])
{
    printColor("phylosofers sit to eat\n", 0x00FFFF);
   /* if ((sem = _semOpen(SEM_PHYL, 1)) == -1)
    {
        printColor("Error opening main semaphore in phylo.\n",ROJO);
        return;
    }
    seated = 0;
    for (int i = 0; i < INIT_PHYL; i++)
    {
        if (addPhylo(i) == -1)
        {
            printColor("Error adding initial philosophers.\n", ROJO);
        }
    }
    print("Press 'a' to add or 'r' to remove a philosopher. Press 'q' to exit.\n");
    */
    char c=1;
    while (c != 'q')
    {
        c=getchar();
        c=tolower(c);
        switch (c)
        {
        case 'a':
            if (addPhylo(seated) == -1)
            {
                printColor("Error adding philosopher.\n", ROJO);
            }
            break;
        case 'r':
            if (removePhylo(seated - 1) == -1)
            {
                printColor("Error removing philosopher.\n", ROJO);
            }
            break;
        default:
            // Busca chopsticks ya que tiene hambre.
            makeshiftSleep(60);
            takeChopstick(2);
            // Termino de comer, deja sus chopsticks.
//putChopstick();

            break;
        }
    }
    endTable();
    printState();
    exit();
    return NULL;
}

 int addPhylo(int pIndex)
{
    if (pIndex >= MAX_PHYL || pIndex < 0)
    {
        return -1;
    }
    return 1;
}
 int removePhylo(int pIndex)
{
    if (pIndex >= MAX_PHYL || pIndex <= MIN_PHYL)
        return -1;

    return 1;
}

void endTable()
{
    return;

}

void printState()
{
    for (int i = 0; i < seated; i++)
    {
       (phylos[i].state == EATING) ? print(" E ") : print(" . ");
    }
    print("\n");
}

void takeChopstick(int pIndex)
{
    
}

void update(int pIndex)
{
    
}
int left(int pIndex){
    return 1;
}

int right(int pIndex){
    return 1;
}


