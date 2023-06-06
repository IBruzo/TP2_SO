// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "semaphores.h"

extern void forceTick();

typedef struct
{
    char name[MAX_NAME];
    int lock;
    int value;
    int size;
    int front; // Index del primer elemento en la cola
    int rear;  // Index del ultimo elemento en la cola
    int queueSize;
    int pidQueue[MAX_SIZE];
} sem_t;

typedef struct
{
    sem_t sem;
    int available;
} semEntry_t;

static semEntry_t semTable[MAX_SEM];
static int semLock = 0;

static int findAvailableSpace();
static int enqueueProcess(int pid, sem_t *sem);
static int dequeueProcess(sem_t *sem);
void printSemQueue(int semIndex);
int findSem(char *name);
char *getSemName(int semIndex);

void initSems();

int semCreate(char *name, int initValue)
{
    int pos;
    if ((pos = findAvailableSpace()) != -1)
    {
        // Inicializamos la estructura del semaforo
        strcpy(semTable[pos].sem.name, name);
        semTable[pos].sem.value = initValue;
        semTable[pos].sem.lock = 0;
        semTable[pos].sem.size = 0;
        semTable[pos].sem.front = 0;
        semTable[pos].sem.rear = -1;
        semTable[pos].sem.queueSize = 0;
    }
    return pos;
}

int semOpen(char *name, int initValue)
{
    while (_xchg(&semLock, 1) != 0)
        ;

    int semIndex = findSem(name);
    if (semIndex == -1)
    {
        semIndex = semCreate(name, initValue);
        if (semIndex == -1)
        {
            _xchg(&semLock, 0);
            return -1;
        }
    }

    semTable[semIndex].sem.size++;

    _xchg(&semLock, 0);
    return semIndex;
}

int semClose(char *name)
{
    while (_xchg(&semLock, 1) != 0)
        ;

    int semIndex = findSem(name);
    if (semIndex == -1)
    {
        _xchg(&semLock, 0);
        return -1;
    }

    if ((--semTable[semIndex].sem.size) <= 0)
    {
        semTable[semIndex].available = 1;
    }

    _xchg(&semLock, 0);
    return 1;
}

int semWait(int semIndex)
{
    if (semIndex >= MAX_SEM)
        return -1;

    sem_t *sem = &semTable[semIndex].sem;

    while (_xchg(&sem->lock, 1) != 0)
        ;

    if (sem->value > 0)
    {
        sem->value--;
        _xchg(&sem->lock, 0);
    }
    else
    {
        int pid = getCurrentPid();
        if (enqueueProcess(pid, sem) == -1)
        {
            _xchg(&sem->lock, 0);
            return -1;
        }

        _xchg(&sem->lock, 0);

        block(pid);

        int flag = 0;
        do
        {
            while (_xchg(&sem->lock, 1) != 0)
                ;

            if (sem->value > 0)
            {
                sem->value--;
                flag = 1;
            }

            _xchg(&sem->lock, 0);
            forceTick();
        } while (!flag);
    }
    return 0;
}

int semPost(int semIndex)
{
    if (semIndex >= MAX_SEM)
    {
        return -1;
    }

    sem_t *sem = &semTable[semIndex].sem;
    while (_xchg(&sem->lock, 1) != 0)
        ;

    sem->value++;

    if (sem->queueSize > 0)
    {
        int pid = 0;
        if ((pid = dequeueProcess(sem)) == -1)
        {
            printSemQueue(semIndex);
            _xchg(&sem->lock, 0);
            return -1;
        }
        unblock(pid);
    }

    _xchg(&sem->lock, 0);

    return 0;
}

/****************************************************************************************************/
void printSemQueue(int semIndex)
{
    if (semIndex >= MAX_SEM)
    {
        print("Invalid semaphore index.\n");
        return;
    }

    sem_t *sem = &semTable[semIndex].sem;
    print("Semaphore: %s\n", sem->name);
    print("Queue Size: %d\n", sem->queueSize);

    int i;
    for (i = 0; i < sem->queueSize; i++)
    {
        print("PID: %d\n", sem->pidQueue[i]);
    }
}

void initSems()
{
    int i;
    for (i = 0; i < MAX_SEM; i++)
    {
        semTable[i].available = 1;
    }
}

static int findAvailableSpace()
{
    int i;
    for (i = 0; i < MAX_SEM; i++)
    {
        if (semTable[i].available)
        {
            semTable[i].available = 0;
            return i;
        }
    }
    return -1;
}

int findSem(char *name)
{
    int i;
    for (i = 0; i < MAX_SEM; i++)
    {
        if (!semTable[i].available && strcmp(name, semTable[i].sem.name) == 0)
        {
            return i;
        }
    }
    return -1;
}

int enqueueProcess(int pid, sem_t *sem)
{
    if (sem->queueSize >= MAX_SIZE)
    {
        return -1;
    }

    sem->rear = (sem->rear + 1) % MAX_SIZE;
    sem->pidQueue[sem->rear] = pid;
    sem->queueSize++;

    return 0;
}

int dequeueProcess(sem_t *sem)
{
    if (sem->queueSize == 0)
    {
        return -1;
    }

    int pid = sem->pidQueue[sem->front];
    sem->front = (sem->front + 1) % MAX_SIZE;
    sem->queueSize--;

    return pid;
}

char *getSemName(int semIndex)
{
    if (semIndex >= MAX_SEM)
    {
        return NULL;
    }
    return semTable[semIndex].sem.name;
}
