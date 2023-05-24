#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include "syscalls.h"
#include "list.h"
#include "dlc_list.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

extern listADT PCBTable;
extern list_t route;

static const uint64_t PageSize = 0x1000;
static void *const sampleCodeModuleAddress = (void *)0x400000; // punteros a user land
static void *const sampleDataModuleAddress = (void *)0x500000;

typedef int (*EntryPoint)();
#define BACKGROUND_COLOR 0x002B36
#define USER_TEXT_COLOR 0x95CD28
#define TERMINAL_BLUE 0x12488B
#define ORANGY 0xF66151
#define RED 0xC01C28
#define FONTCOLOR 0xBFECF1
#define CURRENT_CURSOR_COLOR 0x002B36

void *memset(void *destination, int32_t character, uint64_t length);
void *memcpy(void *destination, const void *source, uint64_t length);
void strcpy(char *destination, const char *origin);
int strlen(const char *str);
char *strrev(char *str);
void appendstringColor(char *string, int color);
void appendcharColor(char character, int color);
void drawCursor(int color);
void newline();
void putcharSpecifics(char character, int x, int y, int size, int color);
void updateCursor();
void buildPCB(PCB *block, int PID, int PPID, uint64_t RSB, char state, char priority, int FDArr[], int FDSize);
void printPCBTable(listADT PCBTable);
int cmpInt(PCB n1, PCB n2);
void clearBSS(void *bssAddress, uint64_t bssSize);
void *getStackBase();
void list_print(list_t *list);
void testProcess1();
void testProcess2();
void testProcess3();
void testProcess4();
char *itoa(int i, char *strout, int base);
extern char *cpuVendor(char *result);
extern int sys_getTime(int op);
void print(char *foundation, ...);
void printf(char *foundation, void *parameters[]);
// in de mapa de entrada y salida
extern uint8_t inb(uint16_t port);
// out de mapa de entrada y salida
extern void outb(uint16_t port, uint8_t value);

uint64_t getSP();

uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base);

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr)-offsetof(type, member)))

#endif
