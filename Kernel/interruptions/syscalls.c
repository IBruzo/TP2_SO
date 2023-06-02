#include <syscalls.h>
#include <lib.h>
#include "memoryManager.h"
#include "schedulerLib.h"
#include "scheduler.h"
#include "semaphores.h"
#include "list.h"

static unsigned int processIDs = 4;

static unsigned char regsBuffer[128] = {0};
void sys_write(uint8_t character, uint32_t x, uint32_t y, uint32_t size, uint32_t color)
{
    put_letter(character, x, y, size, color);
}

char sys_getchar()
{
    return getKey();
}

char sys_getLastKey()
{
    return getLastChar();
}

int sys_tick()
{
    // sys_halt();
    return ticks_elapsed();
}

void sys_putSquare(uint32_t x, uint32_t y, uint32_t tam, uint32_t color)
{
    put_square(x, y, tam, color);
}

void sys_halt()
{
    _hlt();
}

void sys_getRegisters()
{
    char registers[16][4] = {"RAX", "RBX", "RDX", "RCX", "RSI", "RDI", "RBP", "RSP", " R8", " R9", "R10", "R11", "R12", "R13", "R14", "R15"};
    char buffer[50];
    for (int i = 0; i < 16; i++)
    {
        put_word(registers[i], 0, 32 + i * 16 * 2, 2, 0xf66151);
        for (int j = 0; j < 8; j++)
        {
            uintToBase(*(regsBuffer + i * 8 + (8 - j - 1)), buffer, 16);
            put_word(buffer, 32 * 3 + j * 32, 32 + i * 16 * 2, 2, 0xf6b351);
        }
    }
    put_word("Presione ESC para salir", 0, 32 + 16 * 16 * 2, 2, 0xf65194);
}

int sys_getTime(int op)
{
    return getTime(op);
}

void sys_beep(int freq, int time)
{
    beeep(freq, time);
}

void sys_sleep(int seconds)
{
    int start = seconds_elapsed();
    while (seconds_elapsed() - start < seconds)
        _hlt();
}

void sys_clearkeybuffer()
{
    clearKeyBuffer();
}

void sys_changeLanguage(int lan)
{
    changeLanguage(lan);
}

void sys_storeRegisters()
{
    char *regs = snapshot();
    memcpy(regsBuffer, regs, 128);
}

void sys_memAccess(uint64_t memDir)
{
    char buffer[128];

    put_word("Direccion Introducida : ", 0, 32, 2, 0xf66151);
    uintToBase(memDir, buffer, 16);
    put_word(buffer, 0, 64, 2, 0xf65194);

    put_word("Direccion Accedida : ", 0, 96, 2, 0xf66151);
    uintToBase(memDir - (memDir % 16), buffer, 16);
    put_word(buffer, 0, 120, 2, 0xf65194);

    // aca podria haber una validacion que evitara memorias del kernel
    unsigned char *realAddress = (unsigned char *)(memDir - (memDir % 16));
    //

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            uintToBase(*(realAddress + 8 * i + j), buffer, 16);
            put_word(buffer, 64 * j, 180 + i * 16 * 2, 2, 0xf6b351);
        }
    }
    put_word("Presione ESC para salir", 0, 212 + 4 * 16 * 2, 2, 0xf65194);
}

char *sys_mem(int unit)
{
    return mem(unit);
}
void *sys_allocMem(int bytes)
{
    /* utlizo el memManager que fue inicializado por el kernel ( kernel.c ) */
    return memAlloc(bytes);
}

void sys_free(void *dir)
{
    memFree(dir);
}

void sys_scroll_up(uint32_t tamY, uint32_t color)
{
    scroll_up_once(tamY, color);
}

int sys_createProcess(void *(*f)(int, char **), int argc, char **argv)
{
    // Reservo la Memoria para el Stack del Proceso
    uint64_t memStart = (uint64_t)sys_allocMem(PAG_SIZE * 2);
    // Añado el proceso a la Ruta del Scheduler
    list_t *newProcess = (list_t *)sys_allocMem(sizeof(list_t));
    newProcess->data = processIDs;
    list_push(&route, newProcess);
    dlcSize++;
    // Añado a el PCB
    PCB *newBlock = (PCB *)sys_allocMem(sizeof(PCB));
    int newBlockFD[] = {0, 1};
    buildPCB(newBlock, processIDs++, getCurrentPid(), (uint64_t)memStart + PAGE_SIZE + sizeof(PCB) - sizeof(char *), READY, 1, newBlockFD, 2);
    insert(PCBTable, newBlock);

    initializeStackFrame(argc, argv, f, processIDs - 1);
    return processIDs - 1;
}

int sys_semCreate(char *name, int initValue)
{
    return semCreate(name, initValue);
}

int sys_semOpen(char *name, int initValue)
{
    return semOpen(name, initValue);
}

int sys_semClose(char *name)
{
    return semClose(name);
}

int sys_semWait(char *name)
{
    return semWait(getSemIndex(name));
}

int sys_semPost(char *name)
{
    return semPost(getSemIndex(name));
}

int sys_getPid()
{
    return getCurrentPid();
}

int sys_increasePriority(int PID)
{
    // print("\nIncreasing Priority of [%d]...\n", PID);
    // printRoute();

    PCB *toIncrease = get(PCBTable, PID);
    if (toIncrease->priority == 5)
        return -1;

    toIncrease->priority++;
    toIncrease->lives++;
    // print("\n---------------------\n");
    // printRoute();
    // printList(PCBTable);
    return toIncrease->priority;
}

int sys_decreasePriority(int PID)
{
    // print("\nDecreasing Priority...\n");
    // printRoute();

    PCB *toDecrease = get(PCBTable, PID);
    if (toDecrease->priority == 5)
        return -1;

    toDecrease->priority--;
    toDecrease->lives++;
    // print("\n---------------------\n");
    // printRoute();

    return toDecrease->priority;
}

int sys_nice(int pid, int prio)
{
    PCB *toChangePrio = get(PCBTable, pid);
    if (!toChangePrio || toChangePrio->priority == prio || prio <= 0 || prio >= 5)
        return 0;
    toChangePrio->priority = prio;
    toChangePrio->lives = prio;
    return 1;
}

void sys_yield()
{
    // printRoute();
    forceTick();
}

int sys_kill(int pid)
{
    // print("Killing Process...\n");
    // print("WANTING TO MURDER PID [%d]\n", pid);

    if (pid == 0 || pid == 1)
        return 0;

    int cantElim = 0; // found a killable process
    Iterator *routeIt = dclCreateIterator(&route);
    list_t *processIt;
    PCB *killedProcess = get(PCBTable, pid);
    while (cantElim != killedProcess->priority)
    {
        processIt = dclNext(routeIt);
        if (processIt->data == pid)
        {
            list_remove(processIt);
            dlcSize--;
            cantElim++;
        }
    }
    if (cantElim == killedProcess->priority)
    {
        // print("Murdering... \n");
        PCB *killedProcess = get(PCBTable, pid);
        killedProcess->state = EXITED;
        killedProcess->priority = 0;
    }
    return cantElim > 0;
}

void sys_exit()
{
    // if ( currentProcess() == peek().CPID ) => unblock papi
    unblock(getCurrentPPid());
    if (peek(&waitQueue) != -1)
    {
        // print("unblocking [%d] \n",getCurrentPPid() );
        pop(&waitQueue);
    }
    // printRoute();
    // print("Exiting Process...\n");
    sys_kill(getCurrentPid());
    // printRoute();
    forceTick();
}

void sys_waitPid(int pid)
{ // padre ejecuta esto y quiere que lo despierten cuando termine de ejecutar PID
    int cPid = getCurrentPid();

    push(&waitQueue, cPid);
    // print("blocking [%d] \n",cPid );
    // push parent pid y pid
    // push(&waitQueue,pid);
    block(cPid);
    forceTick();
}

void sys_block(int pid)
{
    block(pid);
}
void sys_unblock(int pid)
{
    unblock(pid);
}
