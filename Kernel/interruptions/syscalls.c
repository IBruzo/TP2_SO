#include <syscalls.h>
#include <lib.h>
#include <memoryManager.h>

extern int getTime(int op);

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

void sys_free(void *dir, int size)
{
    memFree(dir);
}

void sys_scroll_up(uint32_t tamY, uint32_t color)
{
    scroll_up_once(tamY, color);
}
void sys_createProcess(void *(*function)(int, char **), int argc, char **argv)
{
    // Reservo memoria del nuevo proceso
    uint64_t memStart = sys_allocMem(4096);

    // Creo su correspondiente PCB
    PCB newBlock;
    int newBlockFD[] = {0, 1, 2};
    buildPCB(&newBlock, processIDs++, 0, memStart + 4096 - 22 * 8, READY, 1, newBlockFD, 3); /* el PPID eventualmente se puede obtener con la getpid syscall */
    insert(PCBTable, newBlock);

    // Añado el proceso a los ciclos del Scheduler
    point *newPoint = (point *)sys_allocMem(sizeof(point));
    newPoint->PID = processIDs;
    list_push(&route, &newPoint->link);
    /* esta instruccion no retorna, porque activa el timer tick y pone en la rueda el proceso */
    // print("Building Dummy Stack...\n");
    // uint64_t RSB = buildDummyStack(memStart + 4096, function, argc, argv);

    return;
}
