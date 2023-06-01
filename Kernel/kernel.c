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
#include "scheduler.h"

int main()
{
	// Desactivadoo Interrupciones mientras se configuran las Estructuras
	_cli();

	/* ------------- Table Loading & Creation ------------ */
	load_idt();
	exceptionsBackupValues((uint64_t)sampleCodeModuleAddress, getSP());
	initMemoryManager(heapAddress, MAX_HEAP_SIZE);

	newList(PCBTable);

	initScheduler();

	/* --------------- Creating Kernel PCB -------------- */
	PCB kernelPCB;
	int kernelFD[] = {0};
	buildPCB(&kernelPCB, KERNEL_PID, KERNEL_PID, 0, BLOCKED, 1, kernelFD, 1);
	insert(PCBTable, &kernelPCB);


	/* ------------ Creating IDLE Process ---------------- */
	// Reserva de memoria para el Stack
	uint64_t *idleMemStart = (uint64_t *)sys_allocMem(PAGE_SIZE);
	
	
	// Creacion de PCB
	PCB idlePCB;
	int idleFD[] = {0};
	buildPCB(&idlePCB, IDLE_PID, KERNEL_PID, (uint64_t)(idleMemStart + PAGE_SIZE - STACK_SIZE), READY, 1, idleFD, sizeof(idleFD) / sizeof(int));
	// Adicion a la PCBT
	insert(PCBTable, &idlePCB);
	// Creacion de Stack
	buildStartUpProcess(idleMemStart + PAGE_SIZE - STACK_SIZE, idleProcess);

	// Transferencia de Control al Userland, se precalcula el valor
	EntryPoint userland = ((EntryPoint)sampleCodeModuleAddress)();
	_sti();
	// _hlt(); <- para mi esto es inutil (joaco)
	return userland; // direccion del _start del userland
}
