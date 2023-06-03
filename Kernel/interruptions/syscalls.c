#include <syscalls.h>
#include <lib.h>
#include "memoryManager.h"
#include "schedulerLib.h"
#include "scheduler.h"
#include "semaphores.h"
#include "list.h"
#include "pipe.h"

static unsigned int processIDs = 4;
static unsigned char regsBuffer[128] = {0};

char sys_write(uint8_t character, uint32_t x, uint32_t y, uint32_t size, uint32_t color)
{
    int currPID = getCurrentPid();
    PCB *currPCB = get(PCBTable, currPID);

    // debugging
    int fd1 = currPCB->FD[0]; // stdin
    int fd2 = currPCB->FD[1]; // stdout

    // output en consola
    if (currPCB->FD[1] == 1)
    {
        put_letter(character, x, y, size, color);
        return 1;
    }
    // output en pipe buffer
    int bytesWritten = pipeWrite(currPCB->FD[1], (const char *)&character);
    if (bytesWritten == -1)
    {
        return -1;
        /* Aca entraria un Background Process ya que no encuentra el FD = -1 */
    }
    return 1;
}

char sys_getchar()
{
    int currPID = getCurrentPid();
    PCB *currPCB = get(PCBTable, currPID);

    // input de consola
    if (currPCB->FD[0] == 0)
    {
        return getKey();

        // input de un buffer
        char buffer[1]; // alloc mem?
        int bytesRead = pipeRead(currPCB->FD[0], buffer, 1);
        if (bytesRead == -1)
        {
            /* Aca entraria un Background Process ya que no encuentra el FD = -1 */
            return 0;
        }
        return buffer[0];
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

    void *sys_mAlloc(int bytes)
    {
        /* utlizo el memManager que fue inicializado por el kernel ( kernel.c ) */
        return memAlloc(bytes);
    }

    void sys_mFree(void *dir)
    {
        memFree(dir);
    }

    void sys_scroll_up(uint32_t tamY, uint32_t color)
    {
        scroll_up_once(tamY, color);
    }

    int sys_createProcess(char *pname, void *(*f)(int, char **), int argc, char **argv, int *fd)
    {
        // Reservo la Memoria para el Stack del Proceso
        uint64_t memStart = (uint64_t)sys_mAlloc(PAG_SIZE * 2);
        // Añado el proceso a la Ruta del Scheduler
        list_t *newProcess = (list_t *)sys_mAlloc(sizeof(list_t));
        newProcess->data = processIDs;
        list_push(&route, newProcess);
        dlcSize++;
        // Añado a el PCB
        PCB *newBlock = (PCB *)sys_mAlloc(sizeof(PCB));
        buildPCB(pname, newBlock, processIDs++, getCurrentPid(), (uint64_t)memStart + PAGE_SIZE + sizeof(PCB) - sizeof(char *), READY, 1, fd);
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
        if (!toChangePrio || prio <= 0 || prio >= 5)
        {
            return -1;
        }
        else if (toChangePrio->priority == prio)
        {
            return 0;
        }
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
        if (pid == 0 || pid == 1)
        {
            return -1;
        }

        int index = 0; // found a killable process

        PCB *killedProcess = get(PCBTable, pid);

        if (killedProcess->state == BLOCKED)
        {
            killedProcess->state = EXITED;
            killedProcess->priority = 0;
            killedProcess->lives = 0;
            return 1;
        }

        Iterator *routeIter = dlcCreateIterator(&route);
        list_t *processIt;
        while (index < dlcSize + 1)
        {
            processIt = dlcNext(routeIter);
            if (processIt->data == killedProcess->PID)
            {
                list_remove(processIt);
                list_t *toFree = processIt;
                memFree(toFree);
                dlcSize--;
                killedProcess->state = EXITED;
                killedProcess->priority = 0;
                killedProcess->lives = 0;
                return 1;
            }
            index++;
        }

        if (killedProcess->state == RUNNING) // caso especifico cuando no esta en dlcList pero esta running
        {
            killedProcess->state = EXITED;
            killedProcess->priority = 0;
            killedProcess->lives = 0;
            return 1;
        }

        // print(" PID: %d %d\n", killedProcess->PID, killedProcess->state);
        return -1;
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

    int sys_block(int pid)
    {

        int ret = block(pid);
        /*  if (pid == getCurrentPid())
        {
            forceTick();
         } */
        return ret;
    }
    int sys_unblock(int pid)
    {
        return unblock(pid);
    }

    void sys_changeInputFD(int pid, int newFD)
    {
        PCB *currPCB = get(PCBTable, pid);
        if (currPCB == NULL)
            return;
        currPCB->FD[0] = newFD;
    }

    void sys_changeOutputFD(int pid, int newFD)
    {
        PCB *currPCB = get(PCBTable, pid);
        if (currPCB == NULL)
            return;
        currPCB->FD[1] = newFD;
    }

    int sys_getInputFD(int pid)
    {
        PCB *currPCB = get(PCBTable, pid);
        if (currPCB == NULL)
            return -1;
        return currPCB->FD[0];
    }

    int sys_getOutputFD(int pid)
    {
        PCB *currPCB = get(PCBTable, pid);
        if (currPCB == NULL)
        {
            return -1;
        }
        return currPCB->FD[1];
    }

    int sys_openPipe(char *name)
    {
        return pipeOpen(name);
    }

    int sys_closePipe(int fd)
    {
        return pipeClose(fd);
    }

    void sys_ps(char *buffer)
    {
        ps(buffer);
    }
    void sys_mem(char *buffer, int unit)
    {
        mem(buffer, unit);
    }
