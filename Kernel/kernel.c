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
	/* La mayoria de lo que esta aca se deberia reemplazar por initPCBTable() y similares */

	_cli();
	print("Starting Kernel...\n");

	//----------------------------------------- Table Loading & Creation ----------------------------------------

	load_idt();
	exceptionsBackupValues((uint64_t)sampleCodeModuleAddress, getSP());
	initMemoryManager(heapAddress, MAX_HEAP_SIZE);

	PCBTable = newList(cmpInt); // Creo PCB Table
	initScheduler();

	// Inserto PCB del Kernel como Nodo Centinela
	PCB kernelPCB;
	int kernelFD[] = {0};
	buildPCB(&kernelPCB, 0, 0, 0, BLOCKED, 2, kernelFD, 1);
	insert(PCBTable, kernelPCB);

	//-----------------------------------------Creating Processes-------------------------------------------

	/* ------------ IDLE ---------------- */

	// Reservo memoria para su stack
	uint64_t *idleMemStart = (uint64_t *)sys_allocMem(4096);
	// Creo su PCB
	PCB idlePCB;
	int idleFD[] = {0};
	buildPCB(&idlePCB, 1, 0, (uint64_t)(idleMemStart + 4096 - 19 * 8), READY, 2, idleFD, 1);
	// Lo añado a la PCBT
	insert(PCBTable, idlePCB);
	// Le creo un Stack acorde
	buildStartUpProcess(idleMemStart + 4096 - 19 * 8, idleProcess);

	/* ------------ TestP1 -------------- */

	// Reservo memoria para su stack
	/* uint64_t *pr1MemStart = (uint64_t *)sys_allocMem(4096);
	// Creo su PCB
	PCB process1;
	int pr1FD[] = {0};
	buildPCB(&process1, 12, 0, (uint64_t)(pr1MemStart + 4096 - 19 * 8), READY, 2, pr1FD, 1);
	// Lo añado a la PCBT
	insert(PCBTable, process1);
	// Le creo un Stack acorde
	buildStartUpProcess(pr1MemStart + 4096 - 19 * 8, testProcess1);
	// Añado el proceso a la ruta del Scheduler

	list_t *pr1 = (list_t *)sys_allocMem(sizeof(list_t));
	pr1->data = 12;
	list_push(&route, pr1);
	dlcSize++; */

	/* ------------ TestP2 -------------- */

	/* // Reservo memoria para su stack
	uint64_t *pr2MemStart = (uint64_t *)sys_allocMem(4096);
	// Creo su PCB
	PCB process2;
	int pr2FD[] = {0};
	buildPCB(&process2, 17, 0, (uint64_t)(pr2MemStart + 4096 - 19 * 8), READY, 2, pr2FD, 1);
	// Lo añado a la PCBT
	insert(PCBTable, process2);
	// Le creo un Stack acorde
	buildStartUpProcess(pr2MemStart + 4096 - 19 * 8, testProcess2);
	// Añado el proceso a la ruta del Scheduler
	list_t *pr2 = (list_t *)sys_allocMem(sizeof(list_t));
	pr2->data = 17;
	list_push(&route, pr2);
	dlcSize++; */

	/* ------------ TestP3 -------------- */

	/* // Reservo memoria para su stack
	uint64_t *shellMem = (uint64_t *)sys_allocMem(4096);
	// Creo su PCB
	PCB shell;
	int shellFD[] = {0};
	buildPCB(&shell, 69, 0, (uint64_t)(shellMem + 4096 - 19 * 8), READY, 2, shellFD, 1);
	// Lo añado a la PCBT
	insert(PCBTable, shell);
	// Le creo un Stack acorde
	buildStartUpProcess(shellMem + 4096 - 19 * 8, initializeShell);
	// Añado el proceso a la ruta del Scheduler
	list_t *shell = (list_t *)sys_allocMem(sizeof(list_t));
	shell->data = 69;
	list_push(&route, shell);
	dlcSize++;
 */
	/* ------------------------------------- Printing Structures ------------------------------------------- */

	// printPCBTable(PCBTable);
	EntryPoint userland = ((EntryPoint)sampleCodeModuleAddress)();
	// el calculo este tardaba mucho tiempo aparentemente
	// porque era un llamado a la funcion
	// entonces precalculas el valor
	// y no llega tirar el tiemr tick
	// salio de suerte
	// para mi este halt no es necesario
	// ahora tenemos otro problema igual
	// la shell esta enojada
	// pero ni arrancamos a arreglarlo
	// tengo el pelo mojado y no me puedo poner auris

	// teniamos un tema
	// iba al idle
	// antes de crear ni un proceso
	// creas un proceso de user desde kernel?
	// esto estamos haciendo nosotros BEEN CHILLING NO FUNCA IGUAL MIRA...
	_sti();
	_hlt();
	return userland; // direccion del _start del userland
}

/*  Process Control Block Table */
/*
	PID : numero natural
	PPID : numero natural
	RSB : puntero de 64 bits
	state : READY, RUNNING o BLOCKED
	priority : 0, 1 o 2
	FD : numero natural
	FDSize : numero natural, mantener viva esta variable!
	memInfo.qPages : numero natural
	memInfo.baseAddress : puntero
	memInfo.limit : puntero (?)
*/
