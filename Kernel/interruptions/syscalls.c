// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <syscalls.h>

static unsigned int processIDs = 4;
static unsigned char regsBuffer[128] = {0};

/* -------------------------------------- I/O Management --------------------------------------- */

// Syscall padre de todas las escrituras
char sys_write(uint8_t character, uint32_t x, uint32_t y, uint32_t size, uint32_t color)
{
    int currPID = getCurrentPid();
    PCB *currPCB = get(PCBTable, currPID);

    // output unico en consola
    if (currPCB->FD[1] == 1)
    {
        put_letter(character, x, y, size, color);
    }
    // como las syscalls que pipean tambien desean que se muestre en pantalla se hace ambas funcionalidades
    if (currPCB->FD[1] > 1)
    {
        // output en console
        put_letter(character, x, y, size, color);

        // output en el pipe buffer
        int bytesWritten = pipeWrite(currPCB->FD[1], (const char *)&character);
        if (bytesWritten == -1)
        {
            /* Aca entraria un Background Process ya que no encuentra el FD = -1 */
            return -1;
        }
        return 1;
    }
    return -1;
}

// Syscall padre de lectura, en este caso es excluyente, se lee de consola o se lee del pipe
char sys_getchar()
{
    int currPID = getCurrentPid();
    PCB *currPCB = get(PCBTable, currPID);

    // input de consola
    if (currPCB->FD[0] == 0)
    {
        return getKey();
    }

    // input de un buffer
    char buffer[1];
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

// Syscall padre de rendering visual
void sys_putSquare(uint32_t x, uint32_t y, uint32_t tam, uint32_t color)
{
    put_square(x, y, tam, color);
}

/* -------------------------------------- Time Management --------------------------------------- */

int sys_tick()
{
    return ticks_elapsed();
}

void sys_halt()
{
    _hlt();
}

int sys_getTime(int op)
{
    return getTime(op);
}

// Funcion auxiliar para crear un sleep usando Wait Pid
static void *wakeUp(int argc, char *argv[])
{
    int start = seconds_elapsed();
    int sec = strToInt(argv[0]);
    while (seconds_elapsed() - start < sec)
    {
    }

    sys_exit();
    return NULL;
}

void sys_sleep(int seconds)
{
    int fd[] = {-1, -1};

    char *argd[1];
    argd[0] = sys_mAlloc(sizeof(char));

    sprintf(argd[0], "%d", seconds);
    int pid = sys_createProcess("sleep", wakeUp, 1, argd, fd);
    sys_waitPid(pid);
    sys_mFree(argd[0]);
    forceTick();
}

/* -------------------------------------- Miscellaneous ------------------------------------------ */

void sys_beep(int freq, int time)
{
    beeep(freq, time);
}

void sys_clearkeybuffer()
{
    clearKeyBuffer();
}

void sys_changeLanguage(int lan)
{
    changeLanguage(lan);
}

void sys_scroll_up(uint32_t tamY, uint32_t color)
{
    scroll_up_once(tamY, color);
}

/* -------------------------------------- Memory Management --------------------------------------- */

// Syscall de Reservado de Memoria, su implementacion varia segun la compilacion
void *sys_mAlloc(int bytes)
{
    return memAlloc(bytes);
}

// Syscall de Liberado de Memoria, su implementacion varia segun la compilacion
void sys_mFree(void *dir)
{
    memFree(dir);
}

// Utiliza una funcion de ASM para absorber el estado de los registros cuando fue llamada
void sys_storeRegisters()
{
    char *regs = snapshot();
    memcpy(regsBuffer, regs, 128);
}

// Muestra los bytes subsiguientes a una direccion de memoria, vestigio de Arquitectura de Computadoras, podria refactorizarse con las nuevas funciones de Kernel y sin Magic Numbers
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

// Muestra el estado de los registros, vestigio de Arquitectura de Computadoras, podria refactorizarse con las nuevas funciones de Kernel y sin Magic Numbers
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

void sys_mem(char *buffer, int unit)
{
    mem(buffer, unit);
}

/* -------------------------------------- Semaphore Management ------------------------------------ */

// Syscalls de semaforos
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
    return semWait(findSem(name));
}

int sys_semPost(char *name)
{
    return semPost(findSem(name));
}

/* -------------------------------------- Process Management --------------------------------------- */

// Syscall de Creacion de Procesos
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
    buildPCB(pname, newBlock, processIDs++, getCurrentPid(), (uint64_t)memStart + PAGE_SIZE + sizeof(PCB) - sizeof(char *), (uint64_t)memStart + PAGE_SIZE + sizeof(PCB) - sizeof(char *) + 20 * 8, READY, 1, fd);
    insert(PCBTable, newBlock);
    // Inicializo su Stack
    initializeStackFrame(argc, argv, f, processIDs - 1);
    return processIDs - 1;
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

// Cambia la prioridad de un proceso a una prioridad arbitraria
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

// Cede el CPU
void sys_yield()
{
    forceTick();
}

int sys_kill(int pid)
{
    // Se evita el asesinato del Kernel y el Idle
    if (pid == 0 || pid == 1)
    {
        return -1;
    }

    PCB *killedProcess = get(PCBTable, pid);

    // Caso donde se asesina/exitea un proceso que estaba en la cima del Wait Stack, se desbloquea el padre y se asesina al hijo
    if (peekWaitStack(&waitStack).cpid == killedProcess->PID && peekWaitStack(&waitStack).pid == killedProcess->PPID)
    {
        unblock(killedProcess->PPID);
        popWaitStack(&waitStack);
    }

    int index = 0; // found a killable process

    // Actualiza estado en PCB Table, caso BLOCKED
    if (killedProcess->state == BLOCKED)
    {
        killedProcess->state = EXITED;
        killedProcess->priority = 0;
        killedProcess->lives = 0;
        return 1;
    }

    // Search and Destroy en la Ruta del Scheduler y actualizacion de la PCB Table
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

    // Caso especifico donde no esta en la Ruta del Scheduler pero esta running
    if (killedProcess->state == RUNNING)
    {
        killedProcess->state = EXITED;
        killedProcess->priority = 0;
        killedProcess->lives = 0;
        return 1;
    }
    return -1;
}

void sys_exit()
{
    PCB *curr = get(PCBTable, getCurrentPid());
    // Se evita un caso borde
    if (curr->state == EXITED || getCurrentPid() == 4)
    {
        forceTick();
        return;
    }
    sys_kill(getCurrentPid());
    forceTick();
}

void sys_waitPid(int pid)
{
    PCB *child = get(PCBTable, pid);
    if (child->state == EXITED)
    {
        return;
    }
    int PPid = getCurrentPid();
    pushWaitStack(&waitStack, getCurrentPid(), pid);
    block(PPid);
    forceTick();
}

int sys_block(int pid)
{
    return block(pid);
}

int sys_unblock(int pid)
{
    return unblock(pid);
}

/* -------------------------------------- Pipes & FD Management ----------------------------------- */

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
