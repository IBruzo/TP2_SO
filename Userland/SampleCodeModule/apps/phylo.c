// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "phylo.h"

static int philosophersCount = 0;
static int state[MAX_PHYL] = {0};
static int phyloPid[MAX_PHYL] = {0};
static char processBuffer[3] = {0};
static char *semNames[MAX_PHYL] = {"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9"};

int queue[MAX_PHYL];
int queueSize = 0;

#define LEFT_PHYLO (i + philosophersCount - 1) % philosophersCount
#define RIGHT_PHYLO (i + 1) % philosophersCount

void think()
{
    makeshiftSleep(15);
}

void eat()
{
    makeshiftSleep(15);
}

void *phylo(int argc, char *argv[])
{

    semOpen("mutex", 1);
    while (1)
    {
        think(); // think
        takeForks(argc);
        eat(); // eat
        putForks(argc);
        printState();
    }

    exit();
    return NULL;
}

void *phyloProcess(int argc, char *argv[])
{
    char c = 1;
    print("WELCOME TO THE DINING PHILOSOPHERS\n");
    print("press a--> to add a philosophers\n");
    print("press r--> to remove a philosophers\n");
    print("press c--> clear screen\n");
    print("\n------------------------------------\n");
    print("PRESS THE SPACEBAR TO CONTINUE\n");

    while (c != ' ')
    {
        c = getchar();
    }

    clearScreen();
    printColor("philosophers sit to have dinner...\n", AMARILLO);

    resetState();
    semOpen("mutex", 1);
    semOpen("countMutex", 1);

    for (int i = 0; i < INIT_PHYL; i++)
    {
        addPhylo();
        queue[i] = -1;
    }

    while (c != 'q')
    {
        c = getchar();
        c = toLower(c);
        switch (c)
        {
        case 'a':
            if (addPhylo() == -1)
            {
                printColor("Error adding phylosopher.\n", ROJO);
            }
            else
            {
                printColor("New phylosopher added.\n", AMARILLO);
            }
            break;
        case 'r':
            if (removePhylo() == -1)
            {
                printColor("Error removing philosopher.\n", ROJO);
            }
            else
            {
                printColor("philosophers removed.\n", AMARILLO);
            }
            break;
        case 'c':
            clearScreen();
            print("current state\n");
            break;
        }
    }

    semClose("mutex");
    semClose("countMutex");
    semClose("queueMutex");
    printState();
    endtable();
    exit();
    return NULL;
}

// crea proceso
int addPhylo()
{
    semWait("countMutex");
    if (philosophersCount >= MAX_PHYL)
    {
        semPost("countMutex");
        return -1;
    }

    phyloPid[philosophersCount] = createFGProcess(processBuffer, phylo, philosophersCount, NULL);

    semOpen(semNames[philosophersCount], 1);
    philosophersCount++;
    semPost("countMutex");
    return 1;
}
int removePhylo()
{
    semWait("countMutex");
    if (philosophersCount <= MIN_PHYL)
    {
        semPost("countMutex");
        return -1;
    }
    semClose(semNames[philosophersCount - 1]);

    kill(phyloPid[philosophersCount - 1]);
    philosophersCount--;
    semPost("countMutex");
    return 1;
}

void endtable()
{
    for (int i = 0; i < philosophersCount; i++)
    {
        kill(phyloPid[i]);
    }
}

void printState()
{

    for (int i = 0; i < philosophersCount; i++)
    {
        if (EATING == state[i])
        {
            print("E ");
        }
        else
        {
            print(". ");
        }
    }
    print("\n");
}

void putForks(int i)
{
    semWait("mutex");
    state[i] = THINKING;

    int j;
    for (j = 0; j < queueSize; j++)
    {
        int k = queue[j];

        if (state[k] == WAITING && state[(k + philosophersCount - 1) % philosophersCount] != EATING && state[(k + 1) % philosophersCount] != EATING)
        {
            state[k] = EATING;
            semPost(semNames[k]);
            int iter;
            for (iter = j; iter < queueSize - 1; iter++)
            {
                queue[iter] = queue[iter + 1];
            }
            queueSize--;
            j--;
        }
    }
    semPost("mutex");
}

void takeForks(int i)
{
    semWait("mutex");
    state[i] = WAITING;

    queue[queueSize++] = i;

    test(i);

    if (state[i] != EATING)
    {
        semPost("mutex");
        semWait(semNames[i]);
    }
    else
    {
        queueSize--;
        semPost("mutex");
    }
}

void resetState()
{
    for (int i = 0; i < MAX_PHYL; i++)
    {
        state[i] = THINKING;
    }
}

void test(int i)
{
    if (state[i] == WAITING && state[LEFT_PHYLO] != EATING && state[RIGHT_PHYLO] != EATING)
    {
        state[i] = EATING;
        semPost(semNames[i]);
    }
}