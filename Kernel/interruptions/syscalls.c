#include <syscalls.h>
#include <lib.h>
#include "memoryManager.h"
#include "scheduler_lib.h"
extern int getTime(int op);

static unsigned int processIDs = 4;

static unsigned char regsBuffer[128] = {0};
void sys_write(uint8_t character, uint32_t x, uint32_t y, uint32_t size, uint32_t color)
{
    put_letter(character, x, y, size, color);
}

char sys_getchar()
{
    char ret = getKey();
    while (ret == 0)
    {
        _hlt();
        ret = getKey();
    }

    return ret;
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

void sys_createProcess(void *(*f)(int, char **), int argc, char **argv)
{
    // Reservo la Memoria para el Stack del Proceso
    uint64_t memStart = (uint64_t)sys_allocMem(4096);
    // Añado el proceso a la Ruta del Scheduler
    list_t *newProcess = (list_t *)sys_allocMem(sizeof(list_t));
    newProcess->data = processIDs;
    list_push(&route, newProcess);
    dlcSize++;
    // Añado a el PCB
    PCB newBlock;
    int newBlockFD[] = {0, 1};

    //uint64_t rsb =0 ;
    //#ifndef BUDDY_MM
    //     rsb = (uint64_t)memStart + 4096 - (20 * 8);
    //#else
    //     rsb = (uint64_t)memStart + 4096 - (21 * 8);
    //#endif
    // pcb->rbp = (uint64_t)node + STACK_SIZE + sizeof(processNode) - sizeof(char *);
    buildPCB(&newBlock, processIDs++, 0, (uint64_t)memStart + PAGE_SIZE + sizeof(PCB) - 1, READY, 1, newBlockFD, 3);
    insert(PCBTable, &newBlock);


    // print("rsb: %d\n", rsb);
    // Creo el Stack Virgen y se activa el Timer Tick
    // initializeStackFrame((uint64_t)memStart + 4096, f, argc, argv); //old dumm 
    initializeStackFrame(argc,argv,f,processIDs -1);
    return;
}
