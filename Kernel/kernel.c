// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <interrupts.h>
#include <sound_driver.h>
#include <video_driver.h>
#include <exceptions.h>
#include "dlcList.h"
#include "list.h"
#include "memoryManager.h"
#include "schedulerLib.h"
#include "scheduler.h"
#include "stack.h"
#include "pipe.h"
#include "waitStack.h"
#include "semaphores.h"

void clearBSS(void *bssAddress, uint64_t bssSize);
void *getStackBase();
void *initializeKernelBinary();

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

void kernelProcess() {}
int main()
{
	// Desactivadoo Interrupciones mientras se configuran las Estructuras
	_cli();

	/* ------------- Table Loading & Creation ------------ */
	load_idt();
	exceptionsBackupValues((uint64_t)sampleCodeModuleAddress, getSP());

	initSems();
	initPipes();
	initMemoryManager(heapAddress, MAX_HEAP_SIZE);
	initializeWaitStack(&waitStack);
	initStack(&inputStack);

	newList(PCBTable);

	initScheduler();

	/* --------------- Creacion del Proceso Centinela/Kernel -------------- */
	PCB kernelPCB;
	int kernelFD[] = {0, 1};
	buildPCB("Alpha", &kernelPCB, KERNEL_PID, KERNEL_PID, 0, 0, BLOCKED, 1, kernelFD);
	insert(PCBTable, &kernelPCB);

	/* --------------------- Creacion del Proceso IDLE -------------------- */
	// Reserva de memoria para el Stack
	uint64_t *idleMemStart = (uint64_t *)sys_mAlloc(PAGE_SIZE);
	// Creacion de PCB
	PCB idlePCB;
	int idleFD[] = {0, 1};
	buildPCB("Idle", &idlePCB, IDLE_PID, KERNEL_PID, (uint64_t)(idleMemStart + PAGE_SIZE - STACK_SIZE), (uint64_t)(idleMemStart + PAGE_SIZE - STACK_SIZE) + 20 * 8, READY, 1, idleFD);
	// Adicion a la PCBT
	insert(PCBTable, &idlePCB);
	// Creacion de Stack
	buildStartUpProcess(idleMemStart + PAGE_SIZE - STACK_SIZE, idleProcess);
	// Transferencia de Control al Userland, se precalcula el valor
	EntryPoint userlandEntryPoint = (EntryPoint)sampleCodeModuleAddress;
	int userland = userlandEntryPoint();

	_sti();
	return userland;
}

void clearBSS(void *bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void *getStackBase()
{
	return (void *)((uint64_t)&endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
					- sizeof(uint64_t)					  // Begin at the top of the stack
	);
}

void *initializeKernelBinary()
{
	void *moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress};

	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);
	return getStackBase();
}
