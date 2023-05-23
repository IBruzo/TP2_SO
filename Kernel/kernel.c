#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <interrupts.h>
#include <sound_driver.h>
#include <video_driver.h>
#include <exceptions.h>
#include "list.h"
#include <bitMapADT.h>
#include "memoryManager.h"

void *initializeKernelBinary()
{

	void *moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

/* Primer hilo del Kernel */
int main()
{
	//-----------------------------------------Table Loading---------------------------------------------------

	load_idt();
	exceptionsBackupValues((uint64_t)sampleCodeModuleAddress, getSP());

	//-----------------------------------------Memory Management-----------------------------------------------

	// Nuestro BitMap :
	initBitMap(); // Deberia ser equivalente para el Buddy
	// Implementacion de Alejo :
	// memManager = createMemoryManager((void *)268435456, (void *)0x100000);

	//-----------------------------------------Process Management----------------------------------------------

	print("Memory Management");

	// Creo PCB Table
	listADT PCBTable = newList(cmpInt);
	// Con este PCB del Kernel, Nodo Centinela
	PCB kernelPCB;
	int kernelFD[] = {0};
	buildPCB(&kernelPCB, 0, 0, 0, BLOCKED, 2, kernelFD, 1);
	insert(PCBTable, kernelPCB);
	// Test PCB
	PCB testPCB;
	int testFD[] = {0, 1, 2, 3};
	memInfo testMB;
	buildPCB(&testPCB, 291, 14, 12345678, READY, 1, testFD, 4);
	insert(PCBTable, testPCB);
	buildPCB(&testPCB, 333, 333, 33333333, READY, 1, testFD, 4);
	insert(PCBTable, testPCB);
	buildPCB(&testPCB, 123213, 333, 33333333, READY, 1, testFD, 4);
	insert(PCBTable, testPCB);

	printPCBTable(PCBTable);

	//-----------------------------------------Process Creation----------------------------------------------

	int *args = {"1", "2"};
	sys_createProcess(cmpInt, args, 2, PCBTable);

	// printeo
	printPCBTable(PCBTable);
	return ((EntryPoint)sampleCodeModuleAddress)(); // dirreccion del _start del userland
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
