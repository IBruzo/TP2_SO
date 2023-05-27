#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>

#include <interrupts.h>
#include <sound_driver.h>
#include <video_driver.h>
#include <exceptions.h>
#include "dlc_list.h"
#include "list.h"
#include "memoryManager.h"
#include "scheduler_lib.h"

void *const heapAddress = (void *)0x600000;
#define MAX_HEAP_SIZE (1024 * 1024 * 128) // 128Mb

void *initializeKernelBinary()
{

	void *moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

void buildStartUpProcess(uint64_t *stackStart, void (*f)())
{
	/* construyendole de abajo para arriba el stack, los pushes lo construyen
	de arriba hacia abajo */
	stackStart[0] = 0;	// R15
	stackStart[1] = 0;	// R14
	stackStart[2] = 0;	// R13
	stackStart[3] = 0;	// R12
	stackStart[4] = 0;	// R11
	stackStart[5] = 0;	// R10
	stackStart[6] = 0;	// R9
	stackStart[7] = 0;	// R8
	stackStart[8] = 0;	// RSI, no necesita parametros
	stackStart[9] = 0;	// RDI, no necesita parametros
	stackStart[10] = 0; // RBP
	stackStart[11] = 0; // RDX
	stackStart[12] = 0; // RCX
	stackStart[13] = 0; // RBX
	stackStart[14] = 0; // RAX

	stackStart[15] = (uint64_t)f;					// Entry Point de Idle
	stackStart[16] = 0x8;							// Code Segment
	stackStart[17] = 0x202;							// RFLAGS
	stackStart[18] = (uint64_t)(stackStart + 4096); // RBP previo a la int
	stackStart[19] = 0;								// Stack Segment

	return;
}

void idleProcess()
{
	while (1)
	{
		print("\nArrived at the IDLE Process\n");
		_hlt();
	}
}

void testProcess1()
{
	while (1)
	{
		print("Chris Arrived");
		_hlt();
	}
	return;
}
void testProcess2()
{
	while (1)
	{
		print("Inaki Arrived");
		_hlt();
	}
	return;
}

void testProcess3()
{
	while (1)
	{
		print("Bruhzo Arrived");
		_hlt();
	}
	return;
}

/* Primer hilo del Kernel */
int main()
{
	_cli();
	/* ------------- Table Loading & Creation ------------ */

	load_idt();
	exceptionsBackupValues((uint64_t)sampleCodeModuleAddress, getSP());
	initMemoryManager(heapAddress, MAX_HEAP_SIZE);

	newList(PCBTable);
	initScheduler();

	/* ---------------- Creating Kernel PCB -------------- */
	PCB kernelPCB;
	int kernelFD[] = {0};
	buildPCB(&kernelPCB, 0, 0, 0, BLOCKED, 2, kernelFD, 1);
	insert(PCBTable, &kernelPCB);

	/* ------------ Creating IDLE Process ---------------- */

	// Reservo memoria para su stack
	uint64_t *idleMemStart = (uint64_t *)sys_allocMem(4096);
	// Creo su PCB
	PCB idlePCB;
	int idleFD[] = {0};
	buildPCB(&idlePCB, 1, 0, (uint64_t)(idleMemStart + 4096 - 19 * 8), READY, 2, idleFD, 1);
	// Lo añado a la PCBT
	insert(PCBTable, &idlePCB);
	// Le creo un Stack acorde
	buildStartUpProcess(idleMemStart + 4096 - 19 * 8, idleProcess);

	// Paso de control al Userland
	EntryPoint userland = ((EntryPoint)sampleCodeModuleAddress)();
	_sti();
	_hlt();
	return userland; // direccion del _start del userland
}
