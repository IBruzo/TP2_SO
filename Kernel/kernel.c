#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <interrupts.h>
#include <sound_driver.h>
#include <video_driver.h>
#include <exceptions.h>
#include "list.h"
#include <bitmapMM.h>
#include "memoryManager.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *)0x400000; // punteros a user land
static void *const sampleDataModuleAddress = (void *)0x500000;

typedef int (*EntryPoint)();

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

int cmpInt(PCB *n1, PCB *n2)
{
	return n1->PID - n2->PID;
}

/* Primer hilo del Kernel */
int main()
{
	load_idt();
	exceptionsBackupValues((uint64_t)sampleCodeModuleAddress, getSP());
	/* inicializo el MemManager, lo almaceno en una constante global  */
	/* puse 256Mb de memoria a mapear, en teoria se le da 1Gb pero ni idea */
	/* como direccion inicial puse el comienzo de la Userland segun la tabla del Pure64 */
	/* arrancar otro memory manager como el buffy o el bitmap */
	initBitMap();
	// memManager = createMemoryManager(/* cantidad de memoria */ (void *)268435456, /* comienzo de dicha memoria */ (void *)0x100000);

	print("Schedule Tester\n");
	listADT PCBTable = newList(cmpInt);
	// ENTRY 1
	PCB pcbEntry1;
	pcbEntry1.PID = 12;
	pcbEntry1.PPID = 23;
	pcbEntry1.RSB = 23523423;
	pcbEntry1.state = 3;
	pcbEntry1.priority = 3;
	pcbEntry1.FD[0] = 0;
	pcbEntry1.FD[1] = 1;
	pcbEntry1.FD[2] = 2;
	pcbEntry1.FD[3] = 3;
	pcbEntry1.FDSize = 4;
	memInfo PCBmemInfo1;
	PCBmemInfo1.qPages = 9;
	PCBmemInfo1.baseAddress = 33999234;
	PCBmemInfo1.limit = 3231423;
	pcbEntry1.memInfo = PCBmemInfo1;
	// ENTRY 2
	PCB pcbEntry2;
	pcbEntry2.PID = 3;
	pcbEntry2.PPID = 43;
	pcbEntry2.RSB = 211123423;
	pcbEntry2.state = 1;
	pcbEntry2.priority = 1;
	pcbEntry2.FD[0] = 0;
	pcbEntry2.FD[1] = 1;
	pcbEntry2.FD[2] = 2;
	pcbEntry2.FD[3] = 3;
	pcbEntry2.FDSize = 4;
	memInfo PCBmemInfo2;
	PCBmemInfo2.qPages = 3;
	PCBmemInfo2.baseAddress = 3324234;
	PCBmemInfo2.limit = 33999234;
	pcbEntry2.memInfo = PCBmemInfo2;
	// ENTRY 3
	PCB pcbEntry3;
	pcbEntry3.PID = 212;
	pcbEntry3.PPID = 87;
	pcbEntry3.RSB = 2359993;
	pcbEntry3.state = 2;
	pcbEntry3.priority = 2;
	pcbEntry3.FD[0] = 0;
	pcbEntry3.FD[1] = 1;
	pcbEntry3.FD[2] = 2;
	pcbEntry3.FD[3] = 5;
	pcbEntry3.FDSize = 4;
	memInfo PCBmemInfo3;
	PCBmemInfo3.qPages = 7;
	PCBmemInfo3.baseAddress = 3333333;
	PCBmemInfo3.limit = 2222222;
	pcbEntry3.memInfo = PCBmemInfo3;

	insert(PCBTable, &pcbEntry1);
	insert(PCBTable, &pcbEntry2);
	insert(PCBTable, &pcbEntry3);

	toBegin(PCBTable);
	while (hasNext(PCBTable))
	{
		PCB *elem = next(PCBTable);
		print("pid : %d     \n", elem->PID);
		print("MEMINFO :      \nbaseAddress    %d\n", elem->memInfo.baseAddress);
		print("limit    %d\n", elem->memInfo.limit);
		print("qPages    %d\n", elem->memInfo.qPages);
		print("FD\n");

		for (int i = 0; i < elem->FDSize; i++)
		{
			print("->%d  ", elem->FD[i]);
		}
		print("\n");
	}
	/* probablemente aca es donde tengo que crear el primer proceso del kernel y declarar la PCB */
	return ((EntryPoint)sampleCodeModuleAddress)(); // dirreccion del _start del userland
}
/* define de los STATES, ready blocked running exited zombie  */

/*  Process Control Block Table */
/*
	Una lista con un compare seria lo mas logico, el compare seria basandose en los pids asignados a cada proceso, el pid deberia ser un numero natural ascendente.
	Tambien contiene la memoria otorgada a cada proceso, deberiamos asignarle una pagina a cada proceso de entrada? El proceso en si no tendria acceso a su PCB asi que lo primero que tendria que hacer es pedir mas memoria? Cada proceso recibe como parametro la direccion donde comienza su memoria asignada?
	El estado READY BLOCKED...
	El RSP
*/
