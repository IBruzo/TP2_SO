#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include "syscalls.h"
#include "list.h"
#include "dlc_list.h"
#include "moduleLoader.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;
static void *const sampleCodeModuleAddress = (void *)0x400000; // punteros a user land
static void *const sampleDataModuleAddress = (void *)0x500000;
static void *const heapAddress = (void *)0x600000;

typedef int (*EntryPoint)();
#define BACKGROUND_COLOR 0x002B36
#define USER_TEXT_COLOR 0x95CD28
#define TERMINAL_BLUE 0x12488B
#define ORANGY 0xF66151
#define RED 0xC01C28
#define FONTCOLOR 0xBFECF1
#define CURRENT_CURSOR_COLOR 0x002B36

#define MAX_HEAP_SIZE (1024 * 1024 * 128) // 128Mb
#define PAGE_SIZE 4096                    // 4Kb
#define STACK_SIZE 152                    // Bytes
#define IDLE_PID 1
#define KERNEL_PID 0

// Para poder usar print();
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
char *itoa(int i, char *strout, int base);
void print(char *foundation, ...);
void printf(char *foundation, void *parameters[]);
char * snprintf(char *foundation, ...);




// Funciones Nativas del Kernel
void clearBSS(void *bssAddress, uint64_t bssSize);
void *getStackBase();
void *initializeKernelBinary();
uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base);
extern int sys_getTime(int op);
// ASM
uint64_t getSP();
extern char *cpuVendor(char *result);
extern uint8_t inb(uint16_t port);
extern void outb(uint16_t port, uint8_t value);
char *snapshot();
int buildDummyStack(uint64_t memStart, void *(*function)(int, char **), int argc, char **argv);
#endif
