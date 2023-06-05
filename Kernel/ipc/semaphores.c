#include "semaphores.h"

extern void forceTick();

typedef struct
{
    sem_t sem;
    int available;
} space;

static space semSpaces[MAX_SEM];

static int findAvailableSpace();
static int lockSem = 0;
static int enqeueProcess(int pid, sem_t *sem);
static int dequeueProcess(sem_t *sem);

void printSemQueue(int semIndex)
{
    if (semIndex >= MAX_SEM)
    {
        print("Invalid semaphore index.\n");
        return;
    }

    sem_t *sem = &semSpaces[semIndex].sem;
    print("Semaphore: %s\n", sem->name);
    print("Queue Size: %d\n", sem->listSize);

    process_t *current = sem->first;
    while (current != NULL)
    {
        print("PID: %d\n", current->pid);
        current = current->next;
    }
}

void initSems()
{
    int i;
    for (i = 0; i < MAX_SEM; i++)
    {
        semSpaces[i].available = 1;
    }
}

static int findAvailableSpace()
{
    int i;
    for (i = 0; i < MAX_SEM; i++)
    {
        if (semSpaces[i].available)
        {
            semSpaces[i].available = 0;
            return i;
        }
    }
    return -1; // No habia mas lugar para crear sem.
}

int semCreate(char *name, int initValue)
{
    // print("semcreate ");
    int pos;
    if ((pos = findAvailableSpace()) != -1)
    {
        // Inicializamos la estructura
        strcpy(semSpaces[pos].sem.name, name);
        semSpaces[pos].sem.value = initValue;
        semSpaces[pos].sem.lock = 0;
        semSpaces[pos].sem.first = NULL;
        semSpaces[pos].sem.last = semSpaces[pos].sem.first;
        semSpaces[pos].sem.size = 0;
        semSpaces[pos].sem.listSize = 0;
    }
    return pos;
}

int semOpen(char *name, int initValue)
{
    while (_xchg(&lockSem, 1) != 0)
        ;
    int semIndex = findSem(name);
    if (semIndex == -1)
    {
        semIndex = semCreate(name, initValue);
        if (semIndex == -1)
        {
            _xchg(&lockSem, 0);
            return -1;
        }
    }
    semSpaces[semIndex].sem.size++;
    _xchg(&lockSem, 0);
    return semIndex;
}

int semClose(char *name)
{
    while (_xchg(&lockSem, 1) != 0)
        ;
    int semIndex = findSem(name);
    if (semIndex == -1)
    {
        return -1;
    }
    if ((--semSpaces[semIndex].sem.size) <= 0)
        semSpaces[semIndex].available = 1;
    _xchg(&lockSem, 0);
    return 1;
}

int semWait(int semIndex)
{
    if (semIndex >= MAX_SEM)
        return -1;
    sem_t *sem = &semSpaces[semIndex].sem;

    while (_xchg(&sem->lock, 1) != 0)
        ;
    if (sem->value > 0)
    {
        sem->value--;
        _xchg(&sem->lock, 0);
    }
    else
    {
        // Si el valor es 0 entonces debo poner al proceso a dormir (encolarlo)
        int pid = getCurrentPid();
        if (enqeueProcess(pid, sem) == -1)
        {
            printSemQueue(semIndex);

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
                // print("%d\n", sem->value);
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

    sem_t *sem = &semSpaces[semIndex].sem;
    while (_xchg(&sem->lock, 1) != 0)
        ;
    sem->value++;
    /* print("post %d", sem->value); */
    int pid = 0;
    if (sem->listSize > 0)
    {
        if ((pid = dequeueProcess(sem)) == -1)
        {
            printSemQueue(semIndex);
            _xchg(&sem->lock, 0);
            return -1;
        }
        unblock(pid);
    }
    _xchg(&sem->lock, 0);
    // desbloquear al proceso que esta en la cola y no a si mismo?
    // forceTick();

    return 0;
}

int findSem(char *name)
{
    int i;
    for (i = 0; i < MAX_SEM; i++)
    {
        if (!semSpaces[i].available && strcmp(name, semSpaces[i].sem.name) == 0)
        {
            return i;
        }
    }
    return -1;
}

int enqeueProcess(int pid, sem_t *sem)
{
    process_t *process = memAlloc(sizeof(process_t));
    if (process == NULL)
    {
        return -1;
    }
    process->pid = pid;
    if (sem->listSize == 0)
    {
        sem->first = process;
        sem->last = sem->first;
        process->next = NULL;
    }
    else
    {
        sem->last->next = process;
        process->next = NULL;
        sem->last = process;
    }
    sem->listSize++;
    return 0;
}

int dequeueProcess(sem_t *sem)
{
    if (sem == NULL || sem->first == NULL)
        return -1;
    process_t *current = sem->first;
    int pid = current->pid;
    sem->first = current->next;
    if (sem->first == NULL)
    {
        sem->last = NULL;
    }
    memFree(current);
    sem->listSize--;
    return pid;
}

char *getSemName(int semIndex)
{
    if (semIndex >= MAX_SEM)
    {
        return NULL;
    }
    return semSpaces[semIndex].sem.name;
}
